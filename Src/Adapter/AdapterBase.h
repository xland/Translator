#pragma once
#include <string>
#include <include/Ling.h>
using namespace winrt::Windows::Data::Json;
class AdapterBase
{
public:
	virtual ~AdapterBase() = default;
	virtual JsonObject translate(const std::wstring& text) = 0;
	bool isChinese(const std::wstring& s);
};

