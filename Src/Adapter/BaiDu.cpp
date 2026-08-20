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

std::wstring BaiDu::translate(const std::wstring& text)
{
    auto appId = Setting::get()->getApiConfig(L"baidu", L"appId");
    auto apiKey = Setting::get()->getApiConfig(L"baidu", L"apiKey");

    if (appId.empty() || apiKey.empty()) {
        throw std::runtime_error("Baidu API credentials not configured");
    }

    // 构造 JSON 请求体
    JsonObject body;
    body.SetNamedValue(L"appid", JsonValue::CreateStringValue(appId));
    body.SetNamedValue(L"q", JsonValue::CreateStringValue(text));
    body.SetNamedValue(L"from", JsonValue::CreateStringValue(L"auto"));
    body.SetNamedValue(L"to", JsonValue::CreateStringValue(L"en"));

    // 创建 HTTP 客户端和请求
    HttpClient httpClient;
    HttpRequestMessage request(HttpMethod::Post(), Uri(L"https://fanyi-api.baidu.com/ait/api/aiTextTranslate"));

    // 设置 Header
    request.Headers().Append(L"Authorization", L"Bearer " + apiKey);

    // 设置请求体（Content-Type 由 HttpStringContent 自动设置）
    auto content = HttpStringContent(body.Stringify(), UnicodeEncoding::Utf8, L"application/json");
    request.Content(content);

    // 发送请求并等待响应
    auto response = httpClient.SendRequestAsync(request).get();

    // 检查响应状态
    if (!response.IsSuccessStatusCode()) {
        throw std::runtime_error("HTTP request failed with status: " + 
            std::to_string(static_cast<int>(response.StatusCode())));
    }

    // 读取响应内容
    auto responseBody = response.Content().ReadAsStringAsync().get();
    auto responseStr = std::wstring(responseBody);

    // 解析响应 JSON
    JsonObject responseObj;
    if (!JsonObject::TryParse(responseStr, responseObj)) {
        throw std::runtime_error("Failed to parse response JSON");
    }

    // 提取翻译结果（根据实际 API 响应结构调整）
    // 假设响应格式为 {"result": "翻译结果"} 或类似结构
    // 这里需要根据百度翻译 API 的实际返回格式调整
    if (responseObj.HasKey(L"result")) {
        return std::wstring(responseObj.GetNamedString(L"result"));
    }

    // 如果没有 result 字段，返回原始响应供调试
    return responseStr;
}
