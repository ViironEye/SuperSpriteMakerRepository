#pragma once
#include <string>

// WinAPI Save File dialog (Windows only)
namespace FileDialog
{
    // filterExample: L"PNG Image (*.png)\0*.png\0All Files\0*.*\0"
    bool saveFile(std::wstring& outPath,
        const wchar_t* filter,
        const wchar_t* defaultExt,
        const wchar_t* initialFileName = L"");

    // удобные врапперы
    bool savePNG(std::wstring& outPath, const wchar_t* initialFileName = L"export.png");
    bool saveJPG(std::wstring& outPath, const wchar_t* initialFileName = L"export.jpg");

    bool openImage(std::wstring& outPath);
}