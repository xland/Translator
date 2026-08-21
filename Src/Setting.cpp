#include "pch.h"
#include <include/Ling.h>
#include "ClipboardText.h"
#include "Setting.h"
#include "Win/WinApi.h"
#include "Win/WinResult.h"
#include "Util.h"
#include "App.h"
#include "Adapter/BaiDu.h"

namespace {
    std::unique_ptr<Setting> setting;
    constexpr int capShortcutMsgId{ 100 };
}


Setting::Setting() :dataPath{ initDataPath() }, configPath{ initConfigPath() }
{
    if (std::filesystem::exists(configPath)) {
        auto content = Ling::Util::readFileText(configPath);
        if (content.empty() || content.find_first_not_of(L" \t\r\n") == std::wstring::npos) {
            initDefaultConfig();
            save();
            return;
        }
        JsonObject obj{ nullptr };
        if (JsonObject::TryParse(content, obj)) {
            configObj = obj;
            return;
        }
        MessageBox(nullptr, L"config.json parse error，use default config", L"ScreenCapture", MB_OK | MB_ICONWARNING);
    }
    initDefaultConfig();
}

Setting::~Setting()
{
}

void Setting::init()
{
    auto ptr = new Setting();
    setting.reset(ptr);
    if ((!setting->getApiConfig(L"BaiDu", L"appId").empty() && !setting->getApiConfig(L"BaiDu", L"apiKey").empty())) {
        return;
    }
    if (!setting->getApiConfig(L"HuoShan", L"accessKeyID").empty() && !setting->getApiConfig(L"HuoShan", L"secretAccessKey").empty()) {
        return;
    }
    if (!setting->getApiConfig(L"ALi", L"accessKeyID").empty()) {
        return;
    }
    WinApi::init();
}

void Setting::dispose()
{
    setting.reset();
}

Setting* Setting::get()
{
    return setting.get();
}

std::filesystem::path Setting::getDataPath()
{
    return dataPath; //复制一份路径对象，不允许就地修改
}

const JsonObject Setting::getConfigObj()
{
    return configObj;
}

void Setting::setShortcutKey(const std::wstring& type, const std::vector<std::wstring>& keys)
{
    std::wstring str;
    for (size_t i = 0; i < keys.size(); i++)
    {
        str += L"+" + keys[i];
    }
    str.erase(0,1);
    auto shortcutKey = configObj.GetNamedObject(L"shortcutKey");
    shortcutKey.SetNamedValue(type, JsonValue::CreateStringValue(str));
    auto app = Ling::App::get();
    app->unRegHotKey(capShortcutMsgId);
    app->regHotKey(str, capShortcutMsgId);
    save();
}

std::wstring Setting::getShortcutKey(const std::wstring& type)
{
    // 一路用带默认值的重载：启动时 ensureDefaults 已经补齐过，这里只是别让运行期
    // 意外（配置被外部改动、问了个没配过的 type）变成一次崩溃
    auto obj = configObj.GetNamedObject(L"shortcutKey", nullptr);
    if (!obj) return L"";
    return std::wstring{ obj.GetNamedString(type, L"") };
}

void Setting::setAutoStart(bool autoStart)
{
    std::wstring runKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (autoStart) {
        wchar_t buffer[MAX_PATH];
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        auto curPath = std::filesystem::path(buffer);
        std::wstring commandLine = std::format(L"\"{}\" --auto-start", curPath.wstring());
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, runKey.data(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            RegSetValueEx(hKey, L"Translator", 0, REG_SZ, (const BYTE*)commandLine.data(), (commandLine.size() + 1) * sizeof(wchar_t));
            RegCloseKey(hKey);
        }
    }
    else {
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, runKey.data(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            RegDeleteValue(hKey, L"Translator");
            RegCloseKey(hKey);
        }
    }
    auto common = configObj.GetNamedObject(L"common");
    common.SetNamedValue(L"autoStart", JsonValue::CreateBooleanValue(autoStart));
    save();
}

bool Setting::getAutoStart()
{
    auto common = configObj.GetNamedObject(L"common", nullptr);
    return common && common.GetNamedBoolean(L"autoStart", false);
}

std::filesystem::path Setting::initDataPath()
{
    PWSTR pathTmp;
    auto hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTmp);
    if (FAILED(hr)) {
        _ASSERT_EXPR(FALSE, L"get roaming path，error");
        return L"";
    }
    auto dataPath = std::filesystem::path{ pathTmp };
    CoTaskMemFree(pathTmp);
    dataPath.append("Translator");
    if (!std::filesystem::exists(dataPath)) {
        if (!std::filesystem::create_directories(dataPath)) {
            _ASSERT_EXPR(FALSE, L"create data path，error");
        }
    }
    return dataPath;
}

std::filesystem::path Setting::initConfigPath()
{
    wchar_t buffer[MAX_PATH]{};
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    auto path = std::filesystem::path{ buffer }.parent_path().append(L"config.json");
    if (std::filesystem::exists(path)) return path;
    auto fallback = this->dataPath; //复制一份路径对象，append 会就地改
    return fallback.append(L"config.json");
}

void Setting::save()
{
    std::wstring str{ configObj.Stringify() };
    Ling::Util::saveFile(configPath.wstring(), str);
}

void Setting::initDefaultConfig()
{
    auto [pData, size] = Ling::Util::getRes(L"config.json");
    auto str = Ling::Util::readTextFromBytes(pData, size);
    configObj = JsonObject::Parse(str);
}

long long Setting::getUpdateCheckDay()
{
    auto common = configObj.GetNamedObject(L"common", nullptr);
    if (!common) return 0;
    return static_cast<long long>(common.GetNamedNumber(L"updateCheckDay", 0));
}

void Setting::setUpdateCheckDay(long long day)
{
    auto common = configObj.GetNamedObject(L"common", nullptr);
    if (!common) return;
    common.SetNamedValue(L"updateCheckDay", JsonValue::CreateNumberValue(static_cast<double>(day)));
    save();
}

void Setting::initShortcutKeys()
{
    auto lingApp = Ling::App::get();
    std::wstring capStr{ getShortcutKey(L"cap") };
    if (capStr.empty()) capStr = L"Ctrl+Alt+Z";
    lingApp->regHotKey(capStr, capShortcutMsgId);

    lingApp->onHotKey.add([this](UINT msg) {
        if (msg == capShortcutMsgId) {
            auto result = ClipboardText::grab();
            //MessageBox(nullptr, result.text.data(), L"系统提示", MB_OK | MB_ICONWARNING);
            if (result.ok() && !result.text.empty()) {
                BaiDu baidu;
                auto translated = baidu.translate(result.text);
                new WinResult(translated);
            }
        }
    });
    lingApp->onSecondInstance.add([this]() {

    });
}

std::wstring Setting::getApiConfig(const std::wstring& provider, const std::wstring& key)
{
    auto flag = configObj.HasKey(L"api");
    auto api = configObj.GetNamedObject(L"api", nullptr);
    if (!api) return L"";
    auto providerObj = api.GetNamedObject(provider, nullptr);
    if (!providerObj) return L"";
    return std::wstring{ providerObj.GetNamedString(key, L"") };
}

void Setting::setApiConfig(const std::wstring& provider, const std::wstring& key, const std::wstring& value)
{
    auto api = configObj.GetNamedObject(L"api", nullptr);
    if (!api) return;
    auto providerObj = api.GetNamedObject(provider, nullptr);
    if (!providerObj) return;
    providerObj.SetNamedValue(key, JsonValue::CreateStringValue(value));
    save();
}
