#include "pch.h"
#include "WinApiHuoShan.h"

WinApiHuoShan::WinApiHuoShan(Ling::WinBase* parent) :Ling::Node(parent)
{
    {
        auto box = makeChild<Ling::Node>();
        box->setHeight(39.f);
        box->setFlexDirection(Ling::FlexDirection::Row);
        box->setAlignItems(Ling::Align::Center);

        auto label = box->makeChild<Ling::Label>();
        label->setText(L"注册火山翻译：");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);

        auto btn = box->makeChild<Ling::Button>();
        btn->setText(L"volcengine.com/product/machine-translation");
        btn->setColor(0x597ef7ff);
        btn->setHoverColor(0x597ef7ff);
        btn->onClick.add([this](Ling::Button* btn) {
            std::wstring url{ L"https://www.volcengine.com/product/machine-translation" };
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
        label->setText(L"每月200万字符免费额度（接口尚在对接中）");
        label->setHeightPercent(100.f);
        label->setJustifyContent(Ling::Justify::Center);
        label->setFlexGrow(1.f);


        auto border = makeChild<Ling::Node>();
        border->setHeight(1.f);
        border->setBg(0xE0E0E0FF);
    }
}

WinApiHuoShan::~WinApiHuoShan()
{
}
