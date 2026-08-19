#include "pch.h"
#include <wincodec.h>
#include <shobjidl.h>
#include <format>
#include <fstream>
#include "Util.h"
#include "Setting.h"

using Microsoft::WRL::ComPtr;


std::wstring Util::createFileName(const std::wstring& ext)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return std::format(L"{:04d}{:02d}{:02d}{:02d}{:02d}{:02d}{:03d}.{}",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, ext);
}


std::string Util::convertToStr(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	auto count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), nullptr, 0, nullptr, nullptr);
	if (count <= 0) return std::string();
	std::string str(count, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), str.data(), count, nullptr, nullptr);
	return str;
}

std::array<int, 3> Util::getVerNum(const std::wstring& exePath)
{
	std::array<int, 3> result{ 0,0,0 };
	std::wstring path{ exePath };
	if (path.empty()) {
		std::vector<wchar_t> buf(MAX_PATH);
		auto len = GetModuleFileName(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
		if (len == 0) return result;
		path.assign(buf.data(), len);
	}
	DWORD dummy;
	//版本资源的大小。不是 PE 文件、或者没打版本资源，这里就是 0
	DWORD versionSize = GetFileVersionInfoSize(path.data(), &dummy);
	if (versionSize == 0) return result;
	std::vector<BYTE> versionData(versionSize);
	if (!GetFileVersionInfo(path.data(), 0, versionSize, versionData.data())) return result;
	VS_FIXEDFILEINFO* fileInfo = nullptr;
	UINT fileInfoSize = 0;
	if (!VerQueryValue(versionData.data(), L"\\", reinterpret_cast<void**>(&fileInfo), &fileInfoSize)) return result;
	result[0] = (fileInfo->dwFileVersionMS >> 16) & 0xFFFF;
	result[1] = fileInfo->dwFileVersionMS & 0xFFFF;
	result[2] = (fileInfo->dwFileVersionLS >> 16) & 0xFFFF;
	return result;
}

std::wstring Util::getVer(const std::wstring& exePath)
{
	auto ver = getVerNum(exePath);
	return std::format(L"{}.{}.{}", ver[0], ver[1], ver[2]);
}

std::wstring Util::readTextFile(const std::filesystem::path& path)
{
	std::ifstream file{ path, std::ios::binary };
	if (!file) return L"";
	std::string bytes{ std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{} };
	if (bytes.empty()) return L"";
	// UTF-16LE BOM（程序自己 save 出来的就是这个）：跳过 BOM，按 wchar_t 重新解释。
	// 字节数是奇数说明文件坏了，末尾那半个字符丢掉，后面解析失败会报错
	if (bytes.size() >= 2 && static_cast<unsigned char>(bytes[0]) == 0xFF
		&& static_cast<unsigned char>(bytes[1]) == 0xFE) {
		std::wstring str((bytes.size() - 2) / sizeof(wchar_t), L'\0');
		memcpy(str.data(), bytes.data() + 2, str.size() * sizeof(wchar_t));
		return str;
	}
	// UTF-8 BOM：跳过
	if (bytes.starts_with("\xEF\xBB\xBF")) bytes.erase(0, 3);
	if (bytes.empty()) return L"";
	// 剩下的一律按 UTF-8 认：记事本、VSCode 新建的文件都是这个，纯 ASCII 的 JSON 也照样过
	auto len = MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()), nullptr, 0);
	if (len <= 0) return L"";
	std::wstring str(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()), str.data(), len);
	return str;
}

ComPtr<IDWriteTextLayout> Util::makeTextLayout(const std::wstring& text, float w, float h, float fontSize)
{
	ComPtr<IDWriteTextLayout> layout;
	auto d2d = Ling::D2D::get();
	d2d->dwriteFactory->CreateTextLayout(text.data(), (UINT32)text.length(), d2d->baseTextFormat.Get(), w, h, layout.GetAddressOf());
	if (!layout) return layout;
	layout->SetFontSize(fontSize, { 0,INT_MAX });
	return layout;
}
