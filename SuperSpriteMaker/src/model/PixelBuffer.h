#pragma once

#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include "../enums/PixelFormat.h"
#include "../structs/PixelFormatStruct.h"

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

    uint8_t* data() { return m_pixels.data(); }
    const uint8_t* data() const { return m_pixels.data(); }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int stride() const { return m_stride; }
    PixelFormat format() const { return m_format; }

    bool inBounds(int x, int y) const {
        return x >= 0 && y >= 0 && x < m_width && y < m_height;
    }

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
