#pragma once
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

inline std::string wideToUtf8(const std::wstring& w)
{
#ifdef _WIN32
    if (w.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
        nullptr, 0, nullptr, nullptr);
    std::string out(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
        (char*)out.data(), size, nullptr, nullptr);
    return out;
#else
    return std::string(w.begin(), w.end());
#endif
}