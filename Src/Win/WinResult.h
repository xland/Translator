#pragma once
#include <include/Ling.h>

class WinResult :public Ling::WinBase
{
public:
	WinResult(const std::wstring& source, const std::wstring& result);
	~WinResult();
private:
	void onCreated() override;
	LRESULT onHitTest(const POINT pos) override;
private:
	std::wstring sourceText;
	std::wstring resultText;
	Ling::TextBox* sourceBox{ nullptr };
	Ling::TextBox* resultBox{ nullptr };
};
