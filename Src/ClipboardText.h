#pragma once
#include <include/Ling.h>
#include <string>

// 靠"模拟一次 Ctrl+C，再读剪贴板"拿到当前选中的文本。
// 这条路子在 Electron / Qt / 终端 / 自绘控件里都有效，是 Windows 上覆盖面最广的取词方式，
// 代价是要动用户的剪贴板，所以每个环节都得加保护：事先抬起用户按着的修饰键、靠剪贴板序号
// 判断目标程序到底有没有响应、取完把原内容原样放回去。
class ClipboardText
{
public:
	enum class Status
	{
		Ok,             // 取到了文本（可能是空串 —— 选中的全是空白字符）
		Busy,           // 上一次 grab 还没结束
		InputBlocked,   // SendInput 被拒：前台窗口是管理员权限的，或者被 BlockInput 挡住了
		NoResponse,     // 剪贴板序号一直没变，目标程序压根没响应 Ctrl+C
		NoText,         // 剪贴板变了，但里面不是文本（复制到的是图片、文件之类）
	};

	struct Result
	{
		Status status{ Status::NoResponse };
		std::wstring text;
		bool ok() const { return status == Status::Ok; }
	};

	struct Options
	{
		// 等目标程序响应 Ctrl+C 的总时限。超时就当"这儿不支持复制"
		UINT timeoutMs{ 300 };
		// 轮询剪贴板序号的间隔，为 0 时按 1ms 算
		UINT pollIntervalMs{ 10 };
		// 取完把原剪贴板内容放回去
		bool restore{ true };
		// 恢复时给数据打上"别进剪贴板历史"的标记，免得用户的 Win+V 里多出一条重复项。
		// 注意管不了偷偷复制的那一份 —— 那份数据是目标程序放上去的，我们没法给它加标记
		bool excludeRestoreFromHistory{ true };
		// 备份原剪贴板的字节数上限。剪贴板里可能躺着一张几百兆的位图，全拷一份进内存不值得，
		// 超限的格式直接跳过（宁可丢一个格式，也不能为了它把内存吃穿）
		SIZE_T maxBackupBytes{ 64ull * 1024 * 1024 };
		// 打开剪贴板时用哪个窗口当 owner。传 nullptr 也能用（Util::saveToClipboard 就是这么干的），
		// 但 MSDN 明确说过 owner 为 NULL 时 EmptyClipboard 之后的 SetClipboardData 可能失败，
		// 所以恢复这一步在意可靠性的话，把自己的窗口传进来
		HWND owner{ nullptr };
	};

	// 模拟 Ctrl+C 取当前选中的文本。
	// 会阻塞最多 timeoutMs 毫秒，别直接在 UI 线程上调 —— 放到工作线程里去，
	// 拿到结果再 PostMessage 回 UI 线程
	static Result grab(const Options& opt = {});
};
