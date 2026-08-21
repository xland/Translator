#include "pch.h"
#include "WinTranslate.h"
#include "Adapter/BaiDu.h"
#include "WinResult.h"

WinTranslate::WinTranslate() :Ling::WinBase()
{
	setTitle(L"翻译");
	setSize(500, 280);
	setCenter();
	createNativeWindow();
}

WinTranslate::~WinTranslate()
{
}

void WinTranslate::onCreated()
{
	enableShadow();
	body->setBg(0xFAFAFAFF);
	body->setFlexDirection(Ling::FlexDirection::Column);
	{
		auto titleBar = body->makeChild<Ling::Node>();
		titleBar->setHeight(32.f);
		titleBar->setWidthPercent(100.f);
		titleBar->setBg(0xE6E6E6FF);
		titleBar->setFlexDirection(Ling::FlexDirection::Row);

		auto titleBox = titleBar->makeChild<Ling::Node>();
		titleBox->setJustifyContent(Ling::Justify::Center);
		titleBox->setAlignItems(Ling::Align::FlexStart);
		titleBox->setPaddingLeft(12.f);
		titleBox->setFlexGrow(1.f);
		auto titleText = titleBox->makeChild<Ling::Label>();
		titleText->setText(L"翻译");

		auto closeBtn = titleBar->makeChild<Ling::Button>();
		closeBtn->setSize(42.f, 32.f);
		closeBtn->setHoverColor(0xFFFFFFFF);
		closeBtn->setHoverBg(0xE81123FF);
		closeBtn->setText(L"\ue6e7");
		closeBtn->setFontFamily(L"icon");
		closeBtn->onClick.add([](Ling::Button* btn) {
			btn->win->close();
			});
	}
	{
		auto box = body->makeChild<Ling::Node>();
		box->setFlexGrow(1.0);
		box->setWidthPercent(100.f);
		box->setPadding(12.f, 12.f, 12.f, 6.f);
		box->setFlexDirection(Ling::FlexDirection::Column);
		sourceBox = box->makeChild<Ling::TextBox>();
		sourceBox->setFlexGrow(1.f);
		sourceBox->setWidthPercent(100.f);
		sourceBox->setFontSize(13.f);
		sourceBox->setPadding(8.f);
		sourceBox->setBg(0xFFFFFFFF);
		sourceBox->setBorder(1.f, 0xD9D9D9FF);
		sourceBox->setFlexShrink(1.f);
		sourceBox->setHeight(0.f);
	}
	{
		auto bar = body->makeChild<Ling::Node>();
		bar->setWidthPercent(100.f);
		bar->setFlexDirection(Ling::FlexDirection::Row);
		bar->setJustifyContent(Ling::Justify::End);
		bar->setPadding(12.f, 0.f, 12.f, 12.f);
		auto btn = bar->makeChild<Ling::Button>();
		btn->setSize(80.f, 32.f);
		btn->setBg(0x4096ffFF);
		btn->setHoverBg(0x1677ffFF);
		btn->setColor(0xFFFFFFFF);
		btn->setHoverColor(0xFFFFFFFF);
		btn->setText(L"翻译");
		btn->setBorderRadius(3.f);
		btn->setFontSize(14.f);
		btn->onClick.add([this](Ling::Button* btn) {
			auto text = sourceBox->getText();
			if (text.empty()) return;
			try {
				BaiDu baidu;
				auto result = baidu.translate(text);
				new WinResult(result);
				btn->win->close();
			}
			catch (...) {
				// 翻译失败，不关闭窗口
			}
		});
	}
	show();
	SetForegroundWindow(hwnd);
}

LRESULT WinTranslate::onHitTest(const POINT pos)
{
	POINT pt = pos;
	ScreenToClient(hwnd, &pt);
	if (!isMaximized) {
		auto result = borderHitTest(pt);
		if (result != HTCLIENT) return result;
	}
	if (pt.x > 0 && pt.y > 0 && pt.x < w - 42 * dpi && pt.y < 40 * dpi) {
		return HTCAPTION;
	}
	return HTCLIENT;
}

void WinTranslate::onMinMaxInfo(MINMAXINFO* mmi)
{
	RECT workAreaRect;
	BOOL getWorkAreaSuccess = SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);
	mmi->ptMaxPosition.x = workAreaRect.left;
	mmi->ptMaxPosition.y = workAreaRect.top;
	mmi->ptMaxSize.x = workAreaRect.right - workAreaRect.left;
	mmi->ptMaxSize.y = workAreaRect.bottom - workAreaRect.top;
	mmi->ptMinTrackSize.x = 300;
	mmi->ptMinTrackSize.y = 200;
}
