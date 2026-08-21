#include "pch.h"
#include "WinApiXiaoNiu.h"

WinApiXiaoNiu::WinApiXiaoNiu(Ling::WinBase* parent) :Ling::Node(parent)
{
    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(L"注册小牛翻译：");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        auto btn = box->makeChild<Ling::Button>();
        btn->setText(L"niutrans.com/dev-page?type=text");
        btn->setColor(0x597ef7ff);
        btn->setHoverColor(0x597ef7ff);
        btn->onClick.add([this](Ling::Button* btn) {
            std::wstring url{ L"https://niutrans.com/dev-page?type=text" };
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
        label->setText(L"每日约20万字符免费额度（接口尚在对接中）");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);
        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }
}

WinApiXiaoNiu::~WinApiXiaoNiu()
{
}
