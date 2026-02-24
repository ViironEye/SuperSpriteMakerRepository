#include "ImageWriter.h"
#include "../model/PixelBuffer.h"
#include "../structs/PixelFormatStruct.h"
#include <vector>
#include <cstdint>
#include <cstring>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../stb/stb_image_write.h"

static void makeFlippedCopyRGBA(const PixelBuffer& pb, std::vector<uint8_t>& outRGBA)
{
    const int w = pb.width();
    const int h = pb.height();
    const int bpp = 4;
    outRGBA.resize(size_t(w) * size_t(h) * bpp);

    const uint8_t* src = pb.data();
    const int srcStride = pb.stride();

    for (int y = 0; y < h; ++y)
    {
        const int sy = (h - 1 - y);
        const uint8_t* rowSrc = src + sy * srcStride;
        uint8_t* rowDst = outRGBA.data() + size_t(y) * size_t(w) * bpp;
        std::memcpy(rowDst, rowSrc, size_t(w) * bpp);
    }
}

static void makeRGBFromRGBA_CompositeOnWhite(const PixelBuffer& pb, std::vector<uint8_t>& outRGB, bool flipY)
{
    const int w = pb.width();
    const int h = pb.height();
    outRGB.resize(size_t(w) * size_t(h) * 3);

    for (int y = 0; y < h; ++y)
    {
        int sy = flipY ? (h - 1 - y) : y;

        for (int x = 0; x < w; ++x)
        {
            PixelRGBA8 s = pb.getPixel(x, sy);

            // композит на белый фон: out = s over white
            // white = 255
            float a = s.a / 255.0f;

            uint8_t r = (uint8_t)((s.r * a) + (255.0f * (1.0f - a)));
            uint8_t g = (uint8_t)((s.g * a) + (255.0f * (1.0f - a)));
            uint8_t b = (uint8_t)((s.b * a) + (255.0f * (1.0f - a)));

            size_t idx = (size_t(y) * size_t(w) + size_t(x)) * 3;
            outRGB[idx + 0] = r;
            outRGB[idx + 1] = g;
            outRGB[idx + 2] = b;
        }
    }
}

namespace ImageWriter
{
    bool savePNG(const std::string& path, const PixelBuffer& pb, bool flipY)
    {
        if (pb.format() != PixelFormat::RGBA8)
            return false;

        const int w = pb.width();
        const int h = pb.height();

        if (!flipY)
        {
            // pb.data() уже с нужным направлением
            return stbi_write_png(path.c_str(), w, h, 4, pb.data(), pb.stride()) != 0;
        }
        else
        {
            std::vector<uint8_t> tmp;
            makeFlippedCopyRGBA(pb, tmp);
            return stbi_write_png(path.c_str(), w, h, 4, tmp.data(), w * 4) != 0;
        }
    }

    bool saveJPG(const std::string& path, const PixelBuffer& pb, int quality, bool flipY)
    {
        if (pb.format() != PixelFormat::RGBA8)
            return false;

        if (quality < 1) quality = 1;
        if (quality > 100) quality = 100;

        const int w = pb.width();
        const int h = pb.height();

        std::vector<uint8_t> rgb;
        makeRGBFromRGBA_CompositeOnWhite(pb, rgb, flipY);

        return stbi_write_jpg(path.c_str(), w, h, 3, rgb.data(), quality) != 0;
    }
}