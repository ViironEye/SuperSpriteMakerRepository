#pragma once
#include <string>

class PixelBuffer;

namespace ImageWriter
{
    // PNG: поддерживает RGBA (альфа сохраняется)
    bool savePNG(const std::string& path, const PixelBuffer& pb, bool flipY = true);

    // JPEG: альфы нет, поэтому композитим на фоне (обычно белый)
    bool saveJPG(const std::string& path, const PixelBuffer& pb, int quality = 90, bool flipY = true);
}