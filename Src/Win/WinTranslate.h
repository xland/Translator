#pragma once
#include <include/Ling.h>

using namespace winrt::Windows::Data::Json;

class WinTranslate :public Ling::WinBase
{
public:
	WinTranslate();
	~WinTranslate();
private:
	void onCreated() override;
	LRESULT onHitTest(const POINT pos) override;
	void onMinMaxInfo(MINMAXINFO* mmi) override;
private:
	Ling::TextBox* sourceBox{ nullptr };
};
