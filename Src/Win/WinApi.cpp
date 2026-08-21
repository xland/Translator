#include "pch.h"
#include "WinApi.h"
#include "WinApiBaiDu.h"
#include "WinApiXiaoNiu.h"
#include "WinApiHuoShan.h"
#include "WinApiALi.h"

std::unique_ptr<WinApi> winApi;

WinApi::WinApi() :Ling::WinBase()
{
	onDestroy.add([]() {
		Ling::App::get()->dq.TryEnqueue([]() { winApi.reset(); });
		});
	setTitle(L"API");
	setSize(680, 560);
	setCenter();
	createNativeWindow();
}

WinApi::~WinApi()
{
}

void WinApi::init()
{
	if (winApi) {
		SetForegroundWindow(winApi->hwnd);
		return;
	}
	winApi.reset(new WinApi());
}

void WinApi::dispose()
{
	winApi.reset();
}

void WinApi::onCreated()
{
	enableShadow();
	body->setBg(0xFAFAFAFF);
	body->setFlexDirection(Ling::FlexDirection::Row);
	auto menuBox = body->makeChild<Ling::Node>();
	menuBox->setBg(0xEEEEF0FF);
	menuBox->setWidth(160.f);
	menuBox->setHeightPercent(100.f);
	menuBox->setPaddingTop(40.f);
	initMenuItems(menuBox);

	content = body->makeChild<WinApiBaiDu>();
	content->setFlexGrow(1.0);
	content->setHeightPercent(100.f);
	content->setPaddingTop(40.f);
	content->setPadding(20.f, 40.f, 20.f, 40.f);
	content->setFlexDirection(Ling::FlexDirection::Column);

	auto closeBtn = body->makeChild<Ling::Button>();
	closeBtn->setSize(42.f, 32.f);
	closeBtn->setPositionType(Ling::Position::Absolute);
	closeBtn->setPosition(Ling::Edge::Right, 0);
	closeBtn->setPosition(Ling::Edge::Top, 0);
	closeBtn->setHoverColor(0xFFFFFFFF);
	closeBtn->setHoverBg(0xE81123FF);
	closeBtn->setText(L"\ue6e7");
	closeBtn->setFontFamily(L"icon");
	closeBtn->onClick.add([](Ling::Button* btn) {
		btn->win->close();
		});
	show();
}

void WinApi::initMenuItems(Ling::Node* menuBox)
{
	std::vector<std::wstring> menuNames = { L"百度", L"火山", L"阿里", L"小牛" };
	for (size_t i = 0; i < menuNames.size(); i++)
	{
		auto menuItem = menuBox->makeChild<Ling::Button>();
		menuItem->setFontSize(14.f);
		menuItem->setHeight(40.f);
		if (i == 0) {
			menuItem->setColor(0xFFFFFFFF);
			menuItem->setBg(0x597ef7ff);
			menuItem->setHoverColor(0xFFFFFFFF);
			menuItem->setHoverBg(0x597ef7ff);
			menuItem->setText(menuNames[i]);
		}
		else {
			menuItem->setHoverColor(0x000000ff);
			menuItem->setHoverBg(0xE1E1E3ff);
			menuItem->setText(menuNames[i]);
		}
		menuItem->onClick.add([this](auto menuItem) {this->onMenuItemClick(menuItem); });
		menus.push_back(menuItem);
	}
}

void WinApi::onMenuItemClick(Ling::Button* menuItem)
{
	auto index = Ling::Util::getIndex(menus, menuItem);
	if (index < 0 || index == menuIndex) return;

	auto oldItem = menus[menuIndex];
	oldItem->setColor(0x333333FF);
	oldItem->setBg(0x00000000);
	oldItem->setHoverColor(0x000000ff);
	oldItem->setHoverBg(0xE1E1E3ff);
	menuIndex = index;
	menuItem->setColor(0xFFFFFFFF);
	menuItem->setBg(0x597ef7ff);
	menuItem->setHoverColor(0xFFFFFFFF);
	menuItem->setHoverBg(0x597ef7ff);

	body->removeChild(content);
	if (menuIndex == 0) {
		content = body->makeChild<WinApiBaiDu>();
	}
	else if (menuIndex == 1) {
		content = body->makeChild<WinApiHuoShan>();
	}
	else if (menuIndex == 2) {
		content = body->makeChild<WinApiALi>();
	}
	else if (menuIndex == 3) {
		content = body->makeChild<WinApiXiaoNiu>();
	}
	content->setFlexGrow(1.0);
	content->setHeightPercent(100.f);
	content->setPadding(20.f, 40.f, 20.f, 40.f);
	content->setFlexDirection(Ling::FlexDirection::Column);
}

LRESULT WinApi::onHitTest(const POINT pos)
{
	POINT pt = pos;
	ScreenToClient(hwnd, &pt);
	if (!isMaximized) {
		auto result = borderHitTest(pt);
		if (result != HTCLIENT) return result;
	}
	if (pt.x > 0 && pt.y > 0 && pt.x < w - 32 * dpi && pt.y < 40 * dpi) {
		return HTCAPTION;
	}
	if (pt.x > 0 && pt.y > 40*5*dpi && pt.x < 160 * dpi && pt.y < h) {
		return HTCAPTION;
	}
	return HTCLIENT;
}
