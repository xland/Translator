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
        throw std::runtime_error("Baidu API credentials not configured");
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
        throw std::runtime_error("HTTP request failed with status: " + 
            std::to_string(static_cast<int>(response.StatusCode())));
    }
    auto responseBody = response.Content().ReadAsStringAsync().get();
    auto responseStr = std::wstring(responseBody);
    JsonObject responseObj;
    if (!JsonObject::TryParse(responseStr, responseObj)) {
        throw std::runtime_error("Failed to parse response JSON");
    }
    if (responseObj.HasKey(L"error_code")) {
        throw std::runtime_error("Failed to parse response JSON");
    }
    auto arr = responseObj.GetNamedArray(L"trans_result");
    return arr.GetObjectAt(0);
}
