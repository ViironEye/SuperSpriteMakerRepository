#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace ImageLoader
{
    bool loadRGBA8(const std::string& path, int& outW, int& outH, std::vector<uint8_t>& outPixels);
}