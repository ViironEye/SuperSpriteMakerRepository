#include "ToolAtlas.h"
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

static void drawRect(std::vector<uint8_t>& img, int W, int H, int x, int y, int w, int h, RGBA c)
{
    for (int i = 0; i < w; ++i) { setPx(img, W, H, x + i, y, c); setPx(img, W, H, x + i, y + h - 1, c); }
    for (int i = 0; i < h; ++i) { setPx(img, W, H, x, y + i, c); setPx(img, W, H, x + w - 1, y + i, c); }
}

static void drawLine(std::vector<uint8_t>& img, int W, int H, int x0, int y0, int x1, int y1, RGBA c)
{
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        setPx(img, W, H, x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

static void drawCircle(std::vector<uint8_t>& img, int W, int H, int cx, int cy, int r, RGBA c)
{
    int x = r, y = 0, err = 0;
    while (x >= y) {
        setPx(img, W, H, cx + x, cy + y, c); setPx(img, W, H, cx + y, cy + x, c);
        setPx(img, W, H, cx - y, cy + x, c); setPx(img, W, H, cx - x, cy + y, c);
        setPx(img, W, H, cx - x, cy - y, c); setPx(img, W, H, cx - y, cy - x, c);
        setPx(img, W, H, cx + y, cy - x, c); setPx(img, W, H, cx + x, cy - y, c);
        y++;
        if (err <= 0) err += 2 * y + 1;
        if (err > 0) { x--; err -= 2 * x + 1; }
    }
}

static void drawIcon_Pencil(std::vector<uint8_t>& img, int W, int H, int ox, int oy)
{
    RGBA fg{ 240,240,240,255 };
    drawLine(img, W, H, ox + 6, oy + 24, ox + 24, oy + 6, fg);
    drawLine(img, W, H, ox + 7, oy + 24, ox + 25, oy + 6, fg);
    drawRect(img, W, H, ox + 5, oy + 23, 6, 6, fg); // “òåëî”
}

static void drawIcon_Ink(std::vector<uint8_t>& img, int W, int H, int ox, int oy)
{
    RGBA fg{ 240,240,240,255 };
    drawLine(img, W, H, ox + 16, oy + 6, ox + 8, oy + 18, fg);
    drawLine(img, W, H, ox + 16, oy + 6, ox + 24, oy + 18, fg);
    drawLine(img, W, H, ox + 8, oy + 18, ox + 16, oy + 26, fg);
    drawLine(img, W, H, ox + 24, oy + 18, ox + 16, oy + 26, fg);
    drawCircle(img, W, H, ox + 16, oy + 20, 2, fg);
}

static void drawIcon_Brush(std::vector<uint8_t>& img, int W, int H, int ox, int oy)
{
    RGBA fg{ 240,240,240,255 };
    drawCircle(img, W, H, ox + 16, oy + 16, 8, fg);
    drawLine(img, W, H, ox + 16, oy + 24, ox + 16, oy + 28, fg);
}

static void drawIcon_Eraser(std::vector<uint8_t>& img, int W, int H, int ox, int oy)
{
    RGBA fg{ 240,240,240,255 };
    drawRect(img, W, H, ox + 8, oy + 10, 16, 12, fg);
    drawLine(img, W, H, ox + 10, oy + 22, ox + 22, oy + 22, fg);
}

static void drawIcon_Select(std::vector<uint8_t>& img, int W, int H, int ox, int oy)
{
    RGBA fg{ 240,240,240,255 };
    for (int x = 8; x <= 24; x += 2) { setPx(img, W, H, ox + x, oy + 8, fg); setPx(img, W, H, ox + x, oy + 24, fg); }
    for (int y = 8; y <= 24; y += 2) { setPx(img, W, H, ox + 8, oy + y, fg); setPx(img, W, H, ox + 24, oy + y, fg); }
}

static void drawIcon_Move(std::vector<uint8_t>& img, int W, int H, int ox, int oy)
{
    RGBA fg{ 240,240,240,255 };
    drawLine(img, W, H, ox + 16, oy + 8, ox + 16, oy + 24, fg);
    drawLine(img, W, H, ox + 8, oy + 16, ox + 24, oy + 16, fg);
    drawLine(img, W, H, ox + 16, oy + 8, ox + 14, oy + 10, fg);
    drawLine(img, W, H, ox + 16, oy + 8, ox + 18, oy + 10, fg);

    drawLine(img, W, H, ox + 16, oy + 24, ox + 14, oy + 22, fg);
    drawLine(img, W, H, ox + 16, oy + 24, ox + 18, oy + 22, fg);

    drawLine(img, W, H, ox + 8, oy + 16, ox + 10, oy + 14, fg);
    drawLine(img, W, H, ox + 8, oy + 16, ox + 10, oy + 18, fg);

    drawLine(img, W, H, ox + 24, oy + 16, ox + 22, oy + 14, fg);
    drawLine(img, W, H, ox + 24, oy + 16, ox + 22, oy + 18, fg);
}

static void drawIcon_Shape(std::vector<uint8_t>& img, int W, int H, int ox, int oy)
{
    RGBA fg{ 240,240,240,255 };
    drawRect(img, W, H, ox + 8, oy + 8, 16, 16, fg);
    drawLine(img, W, H, ox + 10, oy + 22, ox + 22, oy + 10, fg);
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
    const float W = cell * 7.0f;
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
    const int cols = 7;
    const int rows = 1;
    const int W = cell * cols;
    const int H = cell * rows;

    std::vector<uint8_t> img(size_t(W) * size_t(H) * 4, 0);

    for (int i = 0; i < cols; ++i) {
        RGBA bg{ (uint8_t)(45 + i * 2), (uint8_t)(45 + i * 2), (uint8_t)(45 + i * 2), 255 };
        fillRect(img, W, H, i * cell, 0, cell, cell, bg);
        drawRect(img, W, H, i * cell, 0, cell, cell, RGBA{ 20,20,20,255 });
    }

    drawIcon_Pencil(img, W, H, 0 * cell, 0);
    drawIcon_Ink(img, W, H, 1 * cell, 0);
    drawIcon_Brush(img, W, H, 2 * cell, 0);
    drawIcon_Eraser(img, W, H, 3 * cell, 0);
    drawIcon_Select(img, W, H, 4 * cell, 0);
    drawIcon_Move(img, W, H, 5 * cell, 0);
    drawIcon_Shape(img, W, H, 6 * cell, 0);

    if (!m_tex)
        glGenTextures(1, &m_tex);

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
