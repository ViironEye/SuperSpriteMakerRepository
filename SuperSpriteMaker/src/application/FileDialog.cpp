#include "FileDialog.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

namespace
{
    // ¬ажно: фильтр в WinAPI должен быть с '\0' разделител€ми и двойным '\0' в конце.
    constexpr const wchar_t* FILTER_PNG =
        L"PNG Image (*.png)\0*.png\0All Files (*.*)\0*.*\0\0";

    constexpr const wchar_t* FILTER_JPG =
        L"JPEG Image (*.jpg;*.jpeg)\0*.jpg;*.jpeg\0All Files (*.*)\0*.*\0\0";

    constexpr const wchar_t* FILTER_IMG =
        L"Images (*.png;*.jpg;*.jpeg)\0*.png;*.jpg;*.jpeg\0"
        L"PNG (*.png)\0*.png\0"
        L"JPEG (*.jpg;*.jpeg)\0*.jpg;*.jpeg\0"
        L"All Files (*.*)\0*.*\0\0";
}

namespace FileDialog
{
    bool saveFile(std::wstring& outPath,
        const wchar_t* filter,
        const wchar_t* defaultExt,
        const wchar_t* initialFileName)
    {
        wchar_t fileBuf[MAX_PATH] = { 0 };

        // initial filename
        if (initialFileName && initialFileName[0] != L'\0')
        {
            wcsncpy_s(fileBuf, initialFileName, _TRUNCATE);
        }

        OPENFILENAMEW ofn{};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr; // можно передать HWND окна, но не об€зательно
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = fileBuf;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrDefExt = defaultExt; // например L"png"
        ofn.Flags =
            OFN_PATHMUSTEXIST |
            OFN_OVERWRITEPROMPT |
            OFN_NOCHANGEDIR; // не мен€ть текущую директорию процесса

        if (GetSaveFileNameW(&ofn))
        {
            outPath = fileBuf;
            return true;
        }
        return false; // cancel или ошибка
    }

    bool savePNG(std::wstring& outPath, const wchar_t* initialFileName)
    {
        return saveFile(outPath, FILTER_PNG, L"png", initialFileName);
    }

    bool saveJPG(std::wstring& outPath, const wchar_t* initialFileName)
    {
        // defaultExt = "jpg" Ч WinAPI сам подставит, если нет расширени€
        return saveFile(outPath, FILTER_JPG, L"jpg", initialFileName);
    }

    bool openImage(std::wstring& outPath)
    {
        wchar_t fileBuf[MAX_PATH] = { 0 };

        OPENFILENAMEW ofn{};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFilter = FILTER_IMG;
        ofn.lpstrFile = fileBuf;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameW(&ofn))
        {
            outPath = fileBuf;
            return true;
        }
        return false;
    }
}
#else
namespace FileDialog
{
    bool saveFile(std::wstring&, const wchar_t*, const wchar_t*, const wchar_t*) { return false; }
    bool savePNG(std::wstring&, const wchar_t*) { return false; }
    bool saveJPG(std::wstring&, const wchar_t*) { return false; }
}
#endif