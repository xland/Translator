#include "pch.h"
#include "WinApiBaiDu.h"

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

        auto textbox = box->makeChild<Ling::TextBox>();
        textbox->setSize(180.f, 26.f);
        textbox->setFontSize(13.f);
        textbox->setPadding(5.f,0.f,5.f,0.f);
        textbox->setBg(0xFCFCFCFF);
        textbox->setVerticalCenter(true);
        textbox->setBorder(1.f, 0xD9D9D9FF);

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

        auto textbox = box->makeChild<Ling::TextBox>();
        textbox->setSize(180.f, 26.f);
        textbox->setFontSize(13.f);
        textbox->setPadding(5.f, 0.f, 5.f, 0.f);
        textbox->setBg(0xFCFCFCFF);
        textbox->setVerticalCenter(true);
        textbox->setBorder(1.f, 0xD9D9D9FF);

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

        auto textbox = box->makeChild<Ling::TextBox>();
        textbox->setSize(80.f, 26.f);
        textbox->setPadding(5.f, 0.f, 5.f, 0.f);
        textbox->setBg(0xFCFCFCFF);
        textbox->setVerticalCenter(true);
        textbox->setBorder(1.f, 0xD9D9D9FF);
        textbox->setText(L"100");

        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }

}

WinApiBaiDu::~WinApiBaiDu()
{
}
