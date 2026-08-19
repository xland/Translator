#pragma once
#include <include/Ling.h>
#include <filesystem>
#include <winrt/Windows.Data.Json.h>
using namespace winrt::Windows::Data::Json;

class Setting
{
public:
	~Setting();
	static void init();
	// 必须在 CoUninitialize 之前调用：configObj 是 WinRT 对象，晚一步释放就是野内存
	static void dispose();
	static Setting* get();
	std::filesystem::path getDataPath();
	const JsonObject getConfigObj();
	void setShortcutKey(const std::wstring& type, const std::vector<std::wstring>& keys);
	std::wstring getShortcutKey(const std::wstring& type);
	void setAutoStart(bool autoStart);
	bool getAutoStart();
	void initShortcutKeys();
	// 上次检查更新是哪一天（std::chrono::days 的计数，即 1970-01-01 以来的天数），
	// 从来没查过返回 0。一天最多查一次服务端，靠它记账 —— 每次空闲都去请求纯属浪费人家的流量
	long long getUpdateCheckDay();
	void setUpdateCheckDay(long long day);
private:
	Setting();
	// 把 config.json 读进 configObj。文件不存在 / 空文件 / 内容不是合法 JSON 都回落到
	// 默认配置，一律不抛异常 —— 往 exe 目录里丢个空的 config.json 就该能用起来。
	// 返回是否真的从文件里读到了配置（没读到就说明内存里这份是默认值，得落盘补上）
	bool loadConfig();
	// 默认配置里有、当前配置里缺的键，补齐。老版本写下的配置文件、用户手工改坏的配置文件
	// 都可能缺键，而代码里有几处是直接按名字取的，缺一个就抛异常。
	// 返回是否补过东西 —— 补过就得落盘
	bool ensureDefaults();
	std::filesystem::path initDataPath();
	// 决定配置文件用哪一份：exe 同目录有 config.json 就用它（绿色版，配置跟着程序走），
	// 否则用 %appdata%\ScreenCapture\config.json。二者只认一个，读哪儿就写哪儿。
	std::filesystem::path initConfigPath();
	void save();
private:
	const std::filesystem::path dataPath;
	// 必须声明在 dataPath 之后：initConfigPath 找不到 exe 同目录的配置时要回落到 dataPath 上，
	// 成员按声明顺序初始化
	const std::filesystem::path configPath;
	JsonObject configObj;
};

