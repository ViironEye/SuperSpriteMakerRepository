#include "ImageImport.h"
#include "SpriteEditor.h"
#include "../model/Sprite.h"
#include "../model/Layer.h"
#include "../model/Cel.h"
#include "../model/PixelBuffer.h"
#include "../structs/PixelFormatStruct.h"
#include "ImageLoader.h"
#include <vector>
#include <algorithm>
#include <cmath>

static inline uint8_t clampU8(int v)
{
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (uint8_t)v;
}

static PixelRGBA8 sampleNearest(const std::vector<uint8_t>& src, int sw, int sh, int x, int y)
{
    x = x < 0 ? 0 : (x > sw - 1 ? sw - 1 : x);
    y = y < 0 ? 0 : (y > sh - 1 ? sh - 1 : y);
    size_t i = (size_t(y) * (size_t)sw + (size_t)x) * 4;
    return PixelRGBA8(src[i + 0], src[i + 1], src[i + 2], src[i + 3]);
}

static PixelRGBA8 sampleBilinear(const std::vector<uint8_t>& src, int sw, int sh, float fx, float fy)
{
    // clamp to edges
    fx = std::max(0.0f, std::min(fx, (float)(sw - 1)));
    fy = std::max(0.0f, std::min(fy, (float)(sh - 1)));

    int x0 = (int)std::floor(fx);
    int y0 = (int)std::floor(fy);
    int x1 = std::min(x0 + 1, sw - 1);
    int y1 = std::min(y0 + 1, sh - 1);

    float tx = fx - x0;
    float ty = fy - y0;

    auto p00 = sampleNearest(src, sw, sh, x0, y0);
    auto p10 = sampleNearest(src, sw, sh, x1, y0);
    auto p01 = sampleNearest(src, sw, sh, x0, y1);
    auto p11 = sampleNearest(src, sw, sh, x1, y1);

    auto lerp = [](float a, float b, float t) { return a + (b - a) * t; };

    float r0 = lerp((float)p00.r, (float)p10.r, tx);
    float g0 = lerp((float)p00.g, (float)p10.g, tx);
    float b0 = lerp((float)p00.b, (float)p10.b, tx);
    float a0 = lerp((float)p00.a, (float)p10.a, tx);

    float r1 = lerp((float)p01.r, (float)p11.r, tx);
    float g1 = lerp((float)p01.g, (float)p11.g, tx);
    float b1 = lerp((float)p01.b, (float)p11.b, tx);
    float a1 = lerp((float)p01.a, (float)p11.a, tx);

    int r = (int)std::lround(lerp(r0, r1, ty));
    int g = (int)std::lround(lerp(g0, g1, ty));
    int b = (int)std::lround(lerp(b0, b1, ty));
    int a = (int)std::lround(lerp(a0, a1, ty));

    return PixelRGBA8(clampU8(r), clampU8(g), clampU8(b), clampU8(a));
}

namespace ImageImport
{
    bool importAsNewLayer(SpriteEditor& editor, const std::string& path, bool nearest)
    {
        Sprite* spr = editor.sprite();
        if (!spr) return false;

        int sw = 0, sh = 0;
        std::vector<uint8_t> src;
        if (!ImageLoader::loadRGBA8(path, sw, sh, src))
            return false;

        const int cw = spr->width();
        const int ch = spr->height();
        if (cw <= 0 || ch <= 0 || sw <= 0 || sh <= 0)
            return false;

        // ---- preserve aspect ratio: fit-inside ----
        float sx = (float)cw / (float)sw;
        float sy = (float)ch / (float)sh;
        float s = std::min(sx, sy);

        // „тобы не получить 0 при очень маленьком canvas:
        int dw = std::max(1, (int)std::lround(sw * s));
        int dh = std::max(1, (int)std::lround(sh * s));

        int ox = (cw - dw) / 2;
        int oy = (ch - dh) / 2;

        // ---- create new layer ----
        int layerNum = spr->layerCount() + 1;
        Layer* L = spr->createLayer("Imported " + std::to_string(layerNum));
        if (!L) return false;

        editor.setActiveLayerIndex(spr->layerCount() - 1);

        const int fi = editor.activeFrameIndex();
        Cel* cel = L->getCel(fi);
        if (!cel || !cel->frame())
            return false;

        PixelBuffer& dst = cel->pixels();
        dst.clear(PixelRGBA8(0, 0, 0, 0));

        // ---- resample into dst with offset ----
        // map dst pixel -> src coordinate
        // dst rect: [ox..ox+dw-1], [oy..oy+dh-1]
        for (int y = 0; y < dh; ++y)
        {
            for (int x = 0; x < dw; ++x)
            {
                int tx = ox + x;
                int ty = oy + y;
                if (!dst.inBounds(tx, ty)) continue;

                if (nearest)
                {
                    int sx_i = (int)std::floor((x + 0.5f) * (float)sw / (float)dw);
                    int sy_i = (int)std::floor((y + 0.5f) * (float)sh / (float)dh);
                    PixelRGBA8 p = sampleNearest(src, sw, sh, sx_i, sy_i);
                    dst.setPixel(tx, ty, p);
                }
                else
                {
                    float fx = ((x + 0.5f) * (float)sw / (float)dw) - 0.5f;
                    float fy = ((y + 0.5f) * (float)sh / (float)dh) - 0.5f;
                    PixelRGBA8 p = sampleBilinear(src, sw, sh, fx, fy);
                    dst.setPixel(tx, ty, p);
                }
            }
        }

        return true;
    }
}