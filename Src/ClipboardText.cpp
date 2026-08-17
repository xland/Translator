#include "pch.h"
#include "ClipboardText.h"
#include <atomic>
#include <vector>

namespace {
	// 备份下来的一份剪贴板数据。data 是我们自己 GlobalAlloc 的副本，
	// 要么在 restore 里交给剪贴板（之后不能再 free），要么由 freeItems 释放
	struct Item
	{
		UINT format{ 0 };
		HGLOBAL data{ nullptr };
	};

	// 剪贴板是全局独占的，抢不到就重试。剪贴板管理器、Office 的后台线程都会短暂占着它，
	// 这类占用都是毫秒级的，重试几次基本都能拿到
	bool openClipboard(HWND owner)
	{
		for (int i = 0; i < 10; i++) {
			if (OpenClipboard(owner)) return true;
			Sleep(5);
		}
		return false;
	}

	// 这个格式存的是不是 HGLOBAL 内存块。
	// CF_BITMAP / CF_METAFILEPICT / CF_ENHMETAFILE / CF_PALETTE 以及私有、GDI 那两段
	// （0x200-0x3FF）存的是句柄而不是内存块，对它们调 GlobalSize / GlobalLock 是未定义行为，
	// 所以备份时一律跳过。丢掉它们的代价不大：位图类内容剪贴板里通常同时躺着 CF_DIB /
	// CF_DIBV5，这两个是 HGLOBAL，备份得到，恢复后系统还能从它们再合成回 CF_BITMAP
	bool isGlobalFormat(UINT format)
	{
		switch (format) {
		case CF_BITMAP:
		case CF_METAFILEPICT:
		case CF_ENHMETAFILE:
		case CF_PALETTE:
		case CF_OWNERDISPLAY:
		case CF_DSPBITMAP:
		case CF_DSPMETAFILEPICT:
		case CF_DSPENHMETAFILE:
			return false;
		}
		if (format >= CF_PRIVATEFIRST && format <= CF_PRIVATELAST) return false;
		if (format >= CF_GDIOBJFIRST && format <= CF_GDIOBJLAST) return false;
		return true;
	}

	HGLOBAL cloneGlobal(HGLOBAL src, SIZE_T size)
	{
		auto srcPtr = GlobalLock(src);
		if (!srcPtr) return nullptr;
		auto dst = GlobalAlloc(GMEM_MOVEABLE, size);
		if (!dst) {
			GlobalUnlock(src);
			return nullptr;
		}
		auto dstPtr = GlobalLock(dst);
		if (!dstPtr) {
			GlobalFree(dst);
			GlobalUnlock(src);
			return nullptr;
		}
		CopyMemory(dstPtr, srcPtr, size);
		GlobalUnlock(dst);
		GlobalUnlock(src);
		return dst;
	}

	void freeItems(std::vector<Item>& items)
	{
		for (auto& item : items) {
			if (item.data) GlobalFree(item.data);
		}
		items.clear();
	}

	// 把当前剪贴板的所有 HGLOBAL 格式拷一份出来。
	// 这里也会把系统合成出来的格式（有 CF_UNICODETEXT 就自动有 CF_TEXT / CF_LOCALE，
	// 有 CF_DIBV5 就自动有 CF_DIB）一起备份下来，纯属多占点内存，内容是一样的，
	// 而想准确判断"哪些是合成的"反倒得把系统那套合成规则抄一遍，不值当
	std::vector<Item> backupClipboard(HWND owner, SIZE_T maxBytes)
	{
		std::vector<Item> items;
		if (!openClipboard(owner)) return items;
		// 先把格式列表整个抄下来，再逐个取数据。不能边枚举边取：延迟渲染的格式一 GetClipboardData
		// 就会去问原主人现渲染，对方往剪贴板里一写，正在进行的枚举就失效了
		std::vector<UINT> formats;
		UINT format = 0;
		while ((format = EnumClipboardFormats(format)) != 0) {
			if (isGlobalFormat(format)) formats.push_back(format);
		}
		SIZE_T total = 0;
		for (auto item : formats) {
			// 延迟渲染的格式，这一下才真的把数据要过来；原主人已经退出就要不到，跳过
			auto handle = GetClipboardData(item);
			if (!handle) continue;
			auto size = GlobalSize(handle);
			if (size == 0) continue;
			if (total + size > maxBytes) continue;
			auto copy = cloneGlobal(handle, size);
			if (!copy) continue;
			total += size;
			items.push_back({ item, copy });
		}
		CloseClipboard();
		return items;
	}

	// 往剪贴板上挂"别记进历史、别同步到云剪贴板"的标记。
	// 这三个是系统认的注册格式：前两个要一个值为 0 的 DWORD，最后那个只看在不在（值无所谓）
	void markExcludedFromHistory()
	{
		static const wchar_t* names[]{
			L"CanIncludeInClipboardHistory",
			L"CanUploadToCloudClipboard",
			L"ExcludeClipboardContentFromMonitorProcessing",
		};
		for (auto name : names) {
			auto format = RegisterClipboardFormatW(name);
			if (format == 0) continue;
			auto handle = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
			if (!handle) continue;
			auto ptr = static_cast<DWORD*>(GlobalLock(handle));
			if (!ptr) {
				GlobalFree(handle);
				continue;
			}
			*ptr = 0;
			GlobalUnlock(handle);
			if (!SetClipboardData(format, handle)) GlobalFree(handle);
		}
	}

	void restoreClipboard(HWND owner, std::vector<Item>& items, bool excludeFromHistory)
	{
		// 一份都没备份到就别碰剪贴板：清空之后没东西可放，等于把用户的剪贴板给弄丢了，
		// 那比留着我们偷偷复制的那份文本还糟
		if (items.empty()) return;
		if (!openClipboard(owner)) {
			freeItems(items);
			return;
		}
		EmptyClipboard();
		if (excludeFromHistory) markExcludedFromHistory();
		for (auto& item : items) {
			// SetClipboardData 成功后 HGLOBAL 就归剪贴板了，不能再 GlobalFree；失败了才要自己释放
			if (!SetClipboardData(item.format, item.data)) GlobalFree(item.data);
			item.data = nullptr;
		}
		CloseClipboard();
		items.clear();
	}

	INPUT makeInput(WORD vk, bool up)
	{
		// 右 Ctrl / 右 Alt / 两个 Win 键的扫描码和左边那个是同一个，得靠 extended 位区分。
		// 右 Shift 不在此列，它有自己的扫描码（0x36）
		bool extended = (vk == VK_RCONTROL || vk == VK_RMENU || vk == VK_LWIN || vk == VK_RWIN);
		INPUT input{};
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = vk;
		// 扫描码一并填上：有些程序（游戏、DirectInput）只认扫描码，不看虚拟键码
		input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC));
		input.ki.dwFlags = (up ? KEYEVENTF_KEYUP : 0) | (extended ? KEYEVENTF_EXTENDEDKEY : 0);
		return input;
	}

	// 发一次 Ctrl+C。
	// 之前先把用户正按着的修饰键抬起来：用户可能正按着 Shift 扩展选择、或者按着 Ctrl 触发我们，
	// 这时候直接发 Ctrl+C，目标程序收到的是 Ctrl+Shift+C（浏览器里那是打开开发者工具）。
	// 抬起来之后不再按回去 —— 用户真松手时系统会再补一个 keyup，多一个 keyup 无害；
	// 而按回去万一用户已经松手了，就留下一个卡住的修饰键，那是真事故。
	// 所有事件塞进一次 SendInput 发出去：SendInput 保证这一批不会被别的线程注入的按键插进来
	bool sendCopy()
	{
		static const WORD modifiers[]{
			VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL,
			VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN,
		};
		std::vector<INPUT> inputs;
		inputs.reserve(_countof(modifiers) + 4);
		for (auto vk : modifiers) {
			if (GetAsyncKeyState(vk) & 0x8000) inputs.push_back(makeInput(vk, true));
		}
		inputs.push_back(makeInput(VK_CONTROL, false));
		inputs.push_back(makeInput('C', false));
		inputs.push_back(makeInput('C', true));
		inputs.push_back(makeInput(VK_CONTROL, true));
		auto count = static_cast<UINT>(inputs.size());
		// 少发或者一个没发出去，就是被拦了：前台窗口权限比我们高（UIPI），或者有人调了 BlockInput
		return SendInput(count, inputs.data(), sizeof(INPUT)) == count;
	}

	// 读剪贴板里的文本。只问 CF_UNICODETEXT —— 源程序只放了 CF_TEXT 的话，
	// 系统会自动合成一份 Unicode 的出来，不用自己操心 ANSI 转换
	bool readClipboardText(HWND owner, std::wstring& text)
	{
		if (!openClipboard(owner)) return false;
		auto handle = GetClipboardData(CF_UNICODETEXT);
		if (!handle) {
			CloseClipboard();
			return false;
		}
		auto ptr = static_cast<const wchar_t*>(GlobalLock(handle));
		if (!ptr) {
			CloseClipboard();
			return false;
		}
		// GlobalSize 给的是分配尺寸，可能比字符串本身长。以结尾的 \0 为准，
		// 同时用 size 兜住"对方没写结尾 \0"的脏数据，免得读出界
		auto limit = GlobalSize(handle) / sizeof(wchar_t);
		text.assign(ptr, wcsnlen(ptr, limit));
		GlobalUnlock(handle);
		CloseClipboard();
		return true;
	}
}

ClipboardText::Result ClipboardText::grab(const Options& opt)
{
	Result result;
	// 一次只能有一个 grab 在跑：两个并发的 grab 会互相把对方备份的剪贴板内容覆盖掉。
	// 鼠标钩子那条路上事件来得又快又密，这个闸门是必须的
	static std::atomic<bool> running{ false };
	if (running.exchange(true)) {
		result.status = Status::Busy;
		return result;
	}
	struct Guard
	{
		std::atomic<bool>& flag;
		~Guard() { flag = false; }
	} guard{ running };

	// 取词前记下序号。判断目标程序到底有没有响应 Ctrl+C，全靠它前后对比 ——
	// 不看序号而是固定 Sleep 一段再读剪贴板的话，遇到不支持复制的地方就会把上一次的旧内容
	// 当成本次选中的文本返回，这是划词软件最经典的一个 bug
	auto before = GetClipboardSequenceNumber();
	auto backup = opt.restore ? backupClipboard(opt.owner, opt.maxBackupBytes) : std::vector<Item>{};

	if (!sendCopy()) {
		freeItems(backup);
		result.status = Status::InputBlocked;
		return result;
	}

	auto interval = opt.pollIntervalMs > 0 ? opt.pollIntervalMs : 1;
	result.status = Status::NoResponse;
	for (UINT elapsed = 0; elapsed < opt.timeoutMs; elapsed += interval) {
		Sleep(interval);
		if (GetClipboardSequenceNumber() == before) continue;
		// 序号变了，但这不等于数据已经就位：目标程序的 EmptyClipboard 也会让序号 +1，
		// 而那一瞬间剪贴板是空的、还被它自己占着。所以读不到就继续等，别急着收摊。
		// 如果它复制的压根不是文本（图片、文件列表），那就一路等到超时，最后报 NoText
		result.status = Status::NoText;
		if (readClipboardText(opt.owner, result.text)) {
			result.status = Status::Ok;
			break;
		}
	}

	// 剪贴板确实被改过才需要恢复。没被改过（NoResponse）就别多写一遍：
	// 白写一次会多触发一轮剪贴板更新通知，也在 Win+V 历史里多留一条
	if (GetClipboardSequenceNumber() != before) {
		restoreClipboard(opt.owner, backup, opt.excludeRestoreFromHistory);
	}
	freeItems(backup);
	return result;
}

ClipboardText::Result ClipboardText::read(HWND owner)
{
	Result result;
	result.status = readClipboardText(owner, result.text) ? Status::Ok : Status::NoText;
	return result;
}
