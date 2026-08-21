#include "pch.h"
#include "BaiDu.h"
#include "../Setting.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Storage.Streams.h>
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Headers;
using namespace winrt::Windows::Storage::Streams;

JsonObject BaiDu::translate(const std::wstring& text)
{
    auto appId = Setting::get()->getApiConfig(L"BaiDu", L"appId");
    auto apiKey = Setting::get()->getApiConfig(L"BaiDu", L"apiKey");
    if (appId.empty() || apiKey.empty()) {
        std::wstring str = L"appId apiKey not set";
        MessageBox(nullptr, str.data(), L"系统提示", MB_OK | MB_ICONWARNING);
    }
    JsonObject body;
    body.SetNamedValue(L"appid", JsonValue::CreateStringValue(appId));
    body.SetNamedValue(L"q", JsonValue::CreateStringValue(text));
    body.SetNamedValue(L"from", JsonValue::CreateStringValue(L"auto"));
    auto to = isChinese(text) ? L"en" : L"zh";
    body.SetNamedValue(L"to", JsonValue::CreateStringValue(to));
    HttpClient httpClient;
    HttpRequestMessage request(HttpMethod::Post(), Uri(L"https://fanyi-api.baidu.com/ait/api/aiTextTranslate"));
    request.Headers().Append(L"Authorization", L"Bearer " + apiKey);
    auto content = HttpStringContent(body.Stringify(), UnicodeEncoding::Utf8, L"application/json");
    request.Content(content);
    auto response = httpClient.SendRequestAsync(request).get();
    if (!response.IsSuccessStatusCode()) {
        auto str = L"HTTP request failed with status: " + std::to_wstring(static_cast<int>(response.StatusCode()));
        MessageBox(nullptr, str.data(), L"系统提示", MB_OK | MB_ICONWARNING);
    }
    auto responseBody = response.Content().ReadAsStringAsync().get();
    auto responseStr = std::wstring(responseBody);
    JsonObject responseObj;
    if (!JsonObject::TryParse(responseStr, responseObj)) {
        std::wstring str = L"Failed to parse response JSON";
        MessageBox(nullptr, str.data(), L"系统提示", MB_OK | MB_ICONWARNING);
    }
    if (responseObj.HasKey(L"error_code")) {
        std::wstring str = L"response JSON has error code";
        MessageBox(nullptr, str.data(), L"系统提示", MB_OK | MB_ICONWARNING);
    }
    auto arr = responseObj.GetNamedArray(L"trans_result");
    return arr.GetObjectAt(0);
}
