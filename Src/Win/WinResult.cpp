#include "pch.h"
#include "WinResult.h"


WinResult::WinResult() :Ling::WinBase()
{
	setTitle(L"API");
	setSize(680, 560);
	setCenter();
	createNativeWindow();
}

WinResult::~WinResult()
{
}

void WinResult::onCreated()
{
	enableShadow();
	body->setBg(0xFAFAFAFF);
	body->setFlexDirection(Ling::FlexDirection::Row);	
	show();
}


LRESULT WinResult::onHitTest(const POINT pos)
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
