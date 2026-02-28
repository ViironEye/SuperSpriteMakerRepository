#include "ToolAtlas.h"
#include "ImageLoader.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cmath>

static inline uint8_t clampU8(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (uint8_t)v;
}

struct RGBA { uint8_t r, g, b, a; };

static void setPx(std::vector<uint8_t>& img, int W, int H, int x, int y, RGBA c)
{
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    size_t i = (size_t(y) * W + x) * 4;
    img[i + 0] = c.r; img[i + 1] = c.g; img[i + 2] = c.b; img[i + 3] = c.a;
}

static void fillRect(std::vector<uint8_t>& img, int W, int H, int x, int y, int w, int h, RGBA c)
{
    for (int yy = 0; yy < h; ++yy)
        for (int xx = 0; xx < w; ++xx)
            setPx(img, W, H, x + xx, y + yy, c);
}

static inline void blendPx(std::vector<uint8_t>& img, int W, int H, int x, int y, RGBA src)
{
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    if (src.a == 0) return;

    size_t i = (size_t(y) * W + x) * 4;

    const int dr = img[i + 0];
    const int dg = img[i + 1];
    const int db = img[i + 2];

    const int sa = src.a;
    const int inv = 255 - sa;

    img[i + 0] = (uint8_t)((src.r * sa + dr * inv) / 255);
    img[i + 1] = (uint8_t)((src.g * sa + dg * inv) / 255);
    img[i + 2] = (uint8_t)((src.b * sa + db * inv) / 255);
    img[i + 3] = 255;
}

static void blitRGBA_Nearest(std::vector<uint8_t>& dst, int dstW, int dstH, int dx, int dy, int drawW, int drawH, 
    const std::vector<uint8_t>& src, int srcW, int srcH)
{
    if (src.empty() || srcW <= 0 || srcH <= 0) return;

    for (int y = 0; y < drawH; ++y)
    {
        int yy = dy + y;
        if (yy < 0 || yy >= dstH) continue;

        int sy = (y * srcH) / drawH;

        for (int x = 0; x < drawW; ++x)
        {
            int xx = dx + x;
            if (xx < 0 || xx >= dstW) continue;

            int sx = (x * srcW) / drawW;

            size_t si = (size_t(sy) * srcW + sx) * 4;
            RGBA px{
                src[si + 0],
                src[si + 1],
                src[si + 2],
                src[si + 3]
            };

            blendPx(dst, dstW, dstH, xx, yy, px);
        }
    }
}

static void drawIconFromFileIntoCell(std::vector<uint8_t>& atlas, int W, int H, int cellX, int cellY, int cell,
    const std::vector<uint8_t>& iconPx, int iconW, int iconH, int padding = 2)
{
    if (iconPx.empty() || iconW <= 0 || iconH <= 0) return;

    int availW = cell - 2 * padding;
    int availH = cell - 2 * padding;
    if (availW <= 0 || availH <= 0) return;

    float sx = float(availW) / float(iconW);
    float sy = float(availH) / float(iconH);
    float s = (sx < sy) ? sx : sy;

    int drawW = std::max(1, int(std::round(iconW * s)));
    int drawH = std::max(1, int(std::round(iconH * s)));

    int dx = cellX + padding + (availW - drawW) / 2;
    int dy = cellY + padding + (availH - drawH) / 2;

    blitRGBA_Nearest(atlas, W, H, dx, dy, drawW, drawH, iconPx, iconW, iconH);
}

static void drawRect(std::vector<uint8_t>& img, int W, int H, int x, int y, int w, int h, RGBA c)
{
    for (int i = 0; i < w; ++i) { setPx(img, W, H, x + i, y, c); setPx(img, W, H, x + i, y + h - 1, c); }
    for (int i = 0; i < h; ++i) { setPx(img, W, H, x, y + i, c); setPx(img, W, H, x + w - 1, y + i, c); }
}

ToolAtlas::~ToolAtlas()
{
    if (m_tex) {
        glDeleteTextures(1, &m_tex);
        m_tex = 0;
    }
}

void ToolAtlas::getUV(int idx, float& u0, float& v0, float& u1, float& v1) const
{
    const float cell = 32.0f;
    const float W = cell * 9.0f;
    const float H = cell * 1.0f;

    float x0 = idx * cell;
    float y0 = 0.0f;

    u0 = x0 / W;
    v0 = y0 / H;
    u1 = (x0 + cell) / W;
    v1 = (y0 + cell) / H;
}

bool ToolAtlas::create()
{
    const int cell = 32;
    const int cols = 9;
    const int rows = 1;
    const int W = cell * cols;
    const int H = cell * rows;

    std::vector<uint8_t> img(size_t(W) * size_t(H) * 4, 0);

    for (int i = 0; i < cols; ++i) {
        RGBA bg{ (uint8_t)(45 + i * 2), (uint8_t)(45 + i * 2), (uint8_t)(45 + i * 2), 255 };
        fillRect(img, W, H, i * cell, 0, cell, cell, bg);
        drawRect(img, W, H, i * cell, 0, cell, cell, RGBA{ 20,20,20,255 });
    }

    const char* names[9] = {
        "pencil.png",
        "ink.png",
        "brush.png",
        "eraser.png",
        "select.png",
        "move.png",
        "shape.png",
        "fill.png",
        "eyedropper.png"
    };

    for (int i = 0; i < cols; ++i)
    {
        int iw = 0, ih = 0;
        std::vector<uint8_t> px;

        std::string p = ImageLoader::iconPath(names[i]);
        ImageLoader::loadRGBA8FromFile(p, iw, ih, px);

        drawIconFromFileIntoCell(img, W, H, i * cell, 0, cell, px, iw, ih, /*padding=*/2);
        
    }

    if (!m_tex) glGenTextures(1, &m_tex);

    glBindTexture(GL_TEXTURE_2D, m_tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}
