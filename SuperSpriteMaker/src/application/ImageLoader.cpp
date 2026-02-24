#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../stb/stb_image.h"

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
}