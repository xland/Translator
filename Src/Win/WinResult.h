#pragma once
#include <include/Ling.h>

using namespace winrt::Windows::Data::Json;

class WinResult :public Ling::WinBase
{
public:
	WinResult(JsonObject obj);
	~WinResult();
private:
	void onCreated() override;
	LRESULT onHitTest(const POINT pos) override;
	void onMinMaxInfo(MINMAXINFO* mmi) override;
private:
	JsonObject obj;
	Ling::TextBox* sourceBox{ nullptr };
	Ling::TextBox* resultBox{ nullptr };
};
