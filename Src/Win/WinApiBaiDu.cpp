#include "pch.h"
#include "WinApiBaiDu.h"
#include "../Setting.h"

WinApiBaiDu::WinApiBaiDu(Ling::WinBase* parent) :Ling::Node(parent)
{
    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(L"注册百度翻译：");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        auto btn = box->makeChild<Ling::Button>();
        btn->setText(L"fanyi-api.baidu.com");
        btn->setColor(0x597ef7ff);
        btn->setHoverColor(0x597ef7ff);
        btn->onClick.add([this](Ling::Button* btn) {
            std::wstring url{ L"https://fanyi-api.baidu.com/" };
            ShellExecute(win->hwnd, L"open", url.data(), nullptr, nullptr, SW_SHOWNORMAL);
            });

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }
    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(L"APP ID：");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        appIdBox = box->makeChild<Ling::TextBox>();
        appIdBox->setSize(260.f, 26.f);
        appIdBox->setFontSize(13.f);
        appIdBox->setPadding(5.f,0.f,5.f,0.f);
        appIdBox->setBg(0xFCFCFCFF);
        appIdBox->setVerticalCenter(true);
        appIdBox->setBorder(1.f, 0xD9D9D9FF);
        appIdBox->setText(Setting::get()->getApiConfig(L"baidu", L"appId"));
        onAppIdChangeToken = appIdBox->onTextChanged.add([this](Ling::TextBox* tb, const std::wstring& text) {
            Setting::get()->setApiConfig(L"baidu", L"appId", text);
        });

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }

    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(L"API Key：");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        apiKeyBox = box->makeChild<Ling::TextBox>();
        apiKeyBox->setSize(260.f, 26.f);
        apiKeyBox->setFontSize(13.f);
        apiKeyBox->setPadding(5.f, 0.f, 5.f, 0.f);
        apiKeyBox->setBg(0xFCFCFCFF);
        apiKeyBox->setVerticalCenter(true);
        apiKeyBox->setBorder(1.f, 0xD9D9D9FF);
        apiKeyBox->setText(Setting::get()->getApiConfig(L"baidu", L"apiKey"));
        onApiKeyChangeToken = apiKeyBox->onTextChanged.add([this](Ling::TextBox* tb, const std::wstring& text) {
            Setting::get()->setApiConfig(L"baidu", L"apiKey", text);
        });

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }

    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(L"优先级：");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        priorityBox = box->makeChild<Ling::TextBox>();
        priorityBox->setSize(80.f, 26.f);
        priorityBox->setPadding(5.f, 0.f, 5.f, 0.f);
        priorityBox->setBg(0xFCFCFCFF);
        priorityBox->setVerticalCenter(true);
        priorityBox->setBorder(1.f, 0xD9D9D9FF);
        auto orderVal = Setting::get()->getApiConfig(L"baidu", L"order");
        priorityBox->setText(orderVal.empty() ? L"100" : orderVal);
        onPriorityChangeToken = priorityBox->onTextChanged.add([this](Ling::TextBox* tb, const std::wstring& text) {
            if (text.empty()) return;
            try {
                std::stol(text);
                Setting::get()->setApiConfig(L"baidu", L"order", text);
            }
            catch (...) {
                // 输入的不是数字，不保存
            }
        });

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }

    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);
        auto label = box->makeChild<Ling::Label>();
        label->setText(L"每月100万字符免费额度，用完将根据优先级使用其他引擎");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);
        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }
}

WinApiBaiDu::~WinApiBaiDu()
{
    if (appIdBox) appIdBox->onTextChanged.remove(onAppIdChangeToken);
    if (apiKeyBox) apiKeyBox->onTextChanged.remove(onApiKeyChangeToken);
    if (priorityBox) priorityBox->onTextChanged.remove(onPriorityChangeToken);
}
