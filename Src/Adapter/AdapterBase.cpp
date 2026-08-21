#include "pch.h"
#include "AdapterBase.h"

bool AdapterBase::isChinese(const std::wstring& s)
{
    for (wchar_t c : s) {
        if ((c >= 0x4E00 && c <= 0x9FFF) ||  // 常用
            (c >= 0x3400 && c <= 0x4DBF) ||  // 扩展 A
            (c >= 0xF900 && c <= 0xFAFF))    // 兼容汉字
            return true;
    }
    return false;
}
