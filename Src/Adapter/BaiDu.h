#pragma once
#include "AdapterBase.h"
class BaiDu :public AdapterBase
{
public:
	std::wstring translate(const std::wstring& text) override;
};

