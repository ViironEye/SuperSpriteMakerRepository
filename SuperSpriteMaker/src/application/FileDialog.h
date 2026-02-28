#pragma once
#include <string>

namespace FileDialog
{
    bool saveFile(std::wstring& outPath, const wchar_t* filter, const wchar_t* defaultExt, const wchar_t* initialFileName = L"");
    bool savePNG(std::wstring& outPath, const wchar_t* initialFileName = L"export.png");
    bool saveJPG(std::wstring& outPath, const wchar_t* initialFileName = L"export.jpg");

    bool openImage(std::wstring& outPath);
}