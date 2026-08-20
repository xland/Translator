#pragma once
#include <include/Ling.h>

class WinResult :public Ling::WinBase
{
public:
	WinResult();
	~WinResult();
private:
	void onCreated() override;
	LRESULT onHitTest(const POINT pos) override;
private:
	int menuIndex{ 0 };
	Ling::Node* content{ nullptr };
};
