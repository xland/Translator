#pragma once
#include "AdapterBase.h"
#include <include/Ling.h>
class BaiDu :public AdapterBase
{
public:
	JsonObject translate(const std::wstring& text) override;
};

