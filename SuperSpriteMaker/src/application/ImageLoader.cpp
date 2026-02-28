#include "ImageLoader.h"
#include <filesystem>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../stb/stb_image.h"

#include <cstdio>
#include <string>

#if defined(_WIN32)
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

#pragma warning (disable:4996)

static std::string getCwd()
{
    char buf[2048];
    if (getcwd(buf, sizeof(buf))) return std::string(buf);
    return std::string();
}

static bool fileExistsFopen(const char* path)
{
    FILE* f = std::fopen(path, "rb");
    if (!f) return false;
    std::fclose(f);
    return true;
}

#include <string>

#if defined(_WIN32)
#include <windows.h>
static std::string getExeDir()
{
    char path[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (len == 0 || len == MAX_PATH) return std::string();

    for (int i = (int)len - 1; i >= 0; --i) {
        if (path[i] == '\\' || path[i] == '/') { path[i] = 0; break; }
    }
    return std::string(path);
}
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
static std::string getExeDir()
{
    char path[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (n <= 0) return std::string();
    path[n] = 0;

    for (int i = (int)n - 1; i >= 0; --i) {
        if (path[i] == '/') { path[i] = 0; break; }
    }
    return std::string(path);
}
#else
static std::string getExeDir()
{
    return std::string();
}
#endif

static std::string joinPath2(const std::string& a, const std::string& b)
{
#if defined(_WIN32)
    const char sep = '\\';
#else
    const char sep = '/';
#endif
    if (a.empty()) return b;
    if (b.empty()) return a;
    if (a.back() == '/' || a.back() == '\\') return a + b;
    return a + sep + b;
}

namespace ImageLoader
{
    bool loadRGBA8(const std::string& path, int& outW, int& outH, std::vector<uint8_t>& outPixels)
    {
        int w = 0, h = 0, comp = 0;
        stbi_uc* data = stbi_load(path.c_str(), &w, &h, &comp, 4); // force RGBA
        if (!data) return false;

        outW = w;
        outH = h;
        outPixels.assign(data, data + (size_t)w * (size_t)h * 4);
        stbi_image_free(data);
        return true;
    }

    static std::string iconRootFromCwd()
    {
        std::string p = joinPath2(getCwd(), "..");
        p = joinPath2(p, "src");
        p = joinPath2(p, "icons");
        return p;
    }

    static std::string iconRootFromExe()
    {
        std::string p = joinPath2(getExeDir(), "..");
        p = joinPath2(p, "..");
        p = joinPath2(p, "SuperSpriteMaker");
        p = joinPath2(p, "src");
        p = joinPath2(p, "icons");
        return p;
    }

    std::string iconPath(const char* filename)
    {
        std::string p1 = joinPath2(iconRootFromCwd(), filename);
        if (fileExistsFopen(p1.c_str()))
            return p1;

        std::string p2 = joinPath2(iconRootFromExe(), filename);
        return p2;
    }

    bool loadRGBA8FromFile(const std::string& path, int& outW, int& outH, std::vector<uint8_t>& outPixels)
    {
        std::cout << "[ImageLoader] cwd=" << getCwd() << "\n";
        std::cout << "[ImageLoader] path=" << path
            << " fopen=" << (fileExistsFopen(path.c_str()) ? "ok" : "fail") << "\n";

        int w = 0, h = 0, comp = 0;
        stbi_uc* data = stbi_load(path.c_str(), &w, &h, &comp, 4);
        if (!data) {
            std::cout << "[ImageLoader] stbi_load failed: " << stbi_failure_reason() << "\n";
            return false;
        }

        outW = w; outH = h;
        outPixels.assign(data, data + (size_t)w * (size_t)h * 4);
        stbi_image_free(data);
        return true;
    }
}