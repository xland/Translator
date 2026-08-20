#include "pch.h"
#include "WinResult.h"

WinResult::WinResult(const std::wstring& source, const std::wstring& result)
	:Ling::WinBase(), sourceText(source), resultText(result)
{
	setTitle(L"翻译结果");
	setSize(500, 320);
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
		titleText->setText(L"翻译结果");

		auto closeBtn = titleBar->makeChild<Ling::Button>();
		closeBtn->setSize(42.f, 32.f);
		closeBtn->setHoverColor(0xFFFFFFFF);
		closeBtn->setHoverBg(0xE81123FF);
		closeBtn->setText(L"\ue62d");
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
		sourceBox->setText(sourceText);
	}
	{
		auto box = body->makeChild<Ling::Node>();
		box->setFlexGrow(1.0);
		box->setWidthPercent(100.f);
		box->setPadding(12.f,6.f,12.f,12.f);
		box->setFlexDirection(Ling::FlexDirection::Column);
		resultBox = box->makeChild<Ling::TextBox>();
		resultBox->setFlexGrow(1.0);
		resultBox->setWidthPercent(100.f);
		resultBox->setFontSize(13.f);
		resultBox->setPadding(8.f);
		resultBox->setBg(0xFFFFFFFF);
		resultBox->setBorder(1.f, 0xD9D9D9FF);
		resultBox->setText(resultText);
	}
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
	if (pt.x > 0 && pt.y > 0 && pt.x < w - 42 * dpi && pt.y < 40 * dpi) {
		return HTCAPTION;
	}
	return HTCLIENT;
}

void WinResult::onMinMaxInfo(MINMAXINFO* mmi)
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
