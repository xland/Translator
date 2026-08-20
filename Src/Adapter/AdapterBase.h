#pragma once
#include <string>
class AdapterBase
{
public:
	virtual ~AdapterBase() = default;
	virtual std::wstring translate(const std::wstring& text) = 0;
};

