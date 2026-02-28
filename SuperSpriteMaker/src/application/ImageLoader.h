#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace ImageLoader
{
    std::string iconPath(const char* filename);
    bool loadRGBA8(const std::string& path, int& outW, int& outH, std::vector<uint8_t>& outPixels);
    bool loadRGBA8FromFile(const std::string& path, int& outW, int& outH, std::vector<uint8_t>& outPixels);
}