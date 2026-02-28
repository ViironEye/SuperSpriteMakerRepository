#pragma once
#include <string>

class PixelBuffer;

namespace ImageWriter
{
    bool savePNG(const std::string& path, const PixelBuffer& pb, bool flipY = true);
    bool saveJPG(const std::string& path, const PixelBuffer& pb, int quality = 90, bool flipY = true);
}