#pragma once
#include <include/Ling.h>
class WinApiBaiDu :public Ling::Node
{
public:
	WinApiBaiDu(Ling::WinBase* parent);
	~WinApiBaiDu();
private:
	Ling::TextBox* appIdBox{ nullptr };
	Ling::TextBox* apiKeyBox{ nullptr };
	Ling::TextBox* priorityBox{ nullptr };
	winrt::event_token onAppIdChangeToken;
	winrt::event_token onApiKeyChangeToken;
	winrt::event_token onPriorityChangeToken;
};
