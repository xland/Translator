#pragma once
#include <include/Ling.h>

class WinApi :public Ling::WinBase
{
public:
	~WinApi();
	static void init();
	static void dispose();
private:
	WinApi();
	void initMenuItems(Ling::Node* menuBox);
	void onCreated() override;
	void onMenuItemClick(Ling::Button* menu);
	LRESULT onHitTest(const POINT pos) override;
private:
	std::vector<Ling::Button*> menus;
	int menuIndex{ 0 };
	Ling::Node* content{ nullptr };
};
