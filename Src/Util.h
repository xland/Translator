#pragma once
#include <include/Ling.h>
#include <fstream>
#include <array>
class Util
{
public:
	// 以当前时间生成默认文件名，精确到毫秒，避免连续保存时重名
	static std::wstring createFileName(const std::wstring& ext);
	// cgif 的 config.path 要 char*，这里做 UTF-8 转换（Ling::Util 只有反向的 convertToWStr）
	static std::string convertToStr(const std::wstring& wstr);
	// 读 exe 的版本资源（Resource.rc 里的 FILEVERSION），取前三段。exePath 传空串就是问自己。
	// 取不到（不是 PE 文件、没有版本资源、文件不存在）一律返回 {0,0,0} —— 自动升级里
	// 用它校验下载下来的文件，所以失败不能抛，得是个"明显不对"的值
	static std::array<int, 3> getVerNum(const std::wstring& exePath = L"");
	// 上面那个的 "2.5.9" 形式
	static std::wstring getVer(const std::wstring& exePath = L"");
	// 读文本文件，自动识别编码：UTF-16LE BOM / UTF-8 BOM / 无 BOM 按 UTF-8。
	// 用户拿记事本建的 JSON 默认是 UTF-8，程序自己 save 出来的是 UTF-16LE，都得认
	static std::wstring readTextFile(const std::filesystem::path& path);
	// 直接建一个 IDWriteTextLayout。Ling 的 Label/Text 走的是 Composition 节点树，
	// 而 CutMask / WinLong 是在自己的 D2D context 上手绘文字，需要裸的 layout。
	static Microsoft::WRL::ComPtr<IDWriteTextLayout> makeTextLayout(const std::wstring& text, float w, float h, float fontSize);
};
