#pragma once

#include <cstdint>
#include <vector>
#include <glad/glad.h>

enum class PixelFormat {
    RGBA8,
    GRAYSCALE8,
    INDEXED8
};

struct PixelRGBA8 {
    uint8_t r, g, b, a;
    PixelRGBA8(uint8_t _r = 0, uint8_t _g = 0, uint8_t _b = 0, uint8_t _a = 255)
        : r(_r), g(_g), b(_b), a(_a) {
    }
};

class PixelBuffer {
public:
    PixelBuffer(int width, int height, PixelFormat fmt = PixelFormat::RGBA8);

    void clear(const PixelRGBA8& color);

    void setPixel(int x, int y, const PixelRGBA8& color);

    PixelRGBA8 getPixel(int x, int y) const;

    void fillRect(int x, int y, int w, int h, const PixelRGBA8& c);

    void blitFrom(const PixelBuffer& src, int sx, int sy, int sw, int sh, int dx, int dy);

    void uploadToGLTexture(GLuint tex) const;

    uint8_t* raw();
    const uint8_t* raw() const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    int stride() const { return m_stride; }
    PixelFormat format() const { return m_format; }

private:
    int bytesPerPixel(PixelFormat f) const;

private:
    int m_width;
    int m_height;
    int m_stride;
    int m_bpp;
    PixelFormat m_format;

    std::vector<uint8_t> m_pixels;
};
