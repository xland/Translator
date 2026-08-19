#include "pch.h"
#include "Tray.h"
#include "App.h"
#include "Win/WinSetting.h"
#include "Setting.h"

namespace {
	static std::unique_ptr<Tray> trayIns;
	static constexpr UINT settingMsg = 163;
	static constexpr UINT exitMsg = 164;
}

Tray::Tray()
{
	auto lingApp = Ling::App::get();
	lingApp->initTray(100, L"Translator");
	Setting::get()->initShortcutKeys();
	// 左键单击 / 双击 都进入截图
	lingApp->onTrayMouseEvent.add([this](bool isDown, bool isRight) {
		if (isDown && !isRight) {
			//WinCap::init();
		}
		else if (isDown && isRight) {
			this->onTrayRightClick();
		}
	});
}

Tray::~Tray()
{
}

void Tray::init()
{
	auto ptr = new Tray();
	trayIns.reset(ptr);
}

Tray* Tray::get()
{
	return trayIns.get();
}

void Tray::onTrayRightClick()
{
	auto menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, settingMsg, L"设置");
	AppendMenu(menu, MF_STRING, exitMsg, L"退出");
	auto menuId = Ling::App::get()->popupMenu(menu);
	if (menuId == settingMsg)
	{
		WinSetting::init();
	}
	else if (menuId == exitMsg)
	{
		Ling::App::get()->quit(0);
	}
}
