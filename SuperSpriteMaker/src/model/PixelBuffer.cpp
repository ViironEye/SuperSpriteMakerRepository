#include "PixelBuffer.h"
#include <cstring>
#include <cassert>

PixelBuffer::PixelBuffer(int width, int height, PixelFormat fmt) : m_width(width), m_height(height), m_format(fmt)
{
    m_bpp = bytesPerPixel(fmt);
    m_stride = m_width * m_bpp;
    m_pixels.resize(m_stride * m_height);
}

int PixelBuffer::bytesPerPixel(PixelFormat f) const 
{
    switch (f) 
    {
        case PixelFormat::RGBA8: return 4;
        case PixelFormat::GRAYSCALE8: return 1;
        case PixelFormat::INDEXED8: return 1;
    }
    return 4;
}

void PixelBuffer::clear(const PixelRGBA8& c) 
{
    if (m_format == PixelFormat::RGBA8) 
    {
        for (int y = 0; y < m_height; y++) 
        {
            PixelRGBA8* row = reinterpret_cast<PixelRGBA8*>( &m_pixels[y * m_stride] );
            for (int x = 0; x < m_width; x++)
                row[x] = c;
        }
    }
    else 
    {
        uint8_t value = c.r;
        std::fill(m_pixels.begin(), m_pixels.end(), value);
    }
}

void PixelBuffer::setPixel(int x, int y, const PixelRGBA8& c) 
{
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) return;

    uint8_t* row = &m_pixels[y * m_stride];

    if (m_format == PixelFormat::RGBA8) 
    {
        PixelRGBA8* px = reinterpret_cast<PixelRGBA8*>(row + x * 4);
        *px = c;
    }
    else 
    {
        row[x] = c.r;
    }
}

PixelRGBA8 PixelBuffer::getPixel(int x, int y) const 
{
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);

    const uint8_t* row = &m_pixels[y * m_stride];

    if (m_format == PixelFormat::RGBA8) 
    {
        const PixelRGBA8* px = reinterpret_cast<const PixelRGBA8*>(row + x * 4);
        return *px;
    }
    else 
    {
        uint8_t v = row[x];
        return PixelRGBA8(v, v, v, 255);
    }
}

void PixelBuffer::fillRect(int x, int y, int w, int h, const PixelRGBA8& c)
{
    if (w <= 0 || h <= 0) return;

    int x0 = std::max(0, x);
    int y0 = std::max(0, y);
    int x1 = std::min(m_width, x + w);
    int y1 = std::min(m_height, y + h);

    for (int yy = y0; yy < y1; yy++) 
    {
        uint8_t* row = &m_pixels[yy * m_stride];

        if (m_format == PixelFormat::RGBA8) 
        {
            PixelRGBA8* px = reinterpret_cast<PixelRGBA8*>(row);
            for (int xx = x0; xx < x1; xx++)
                px[xx] = c;
        }
        else 
        {
            for (int xx = x0; xx < x1; xx++)
                row[xx] = c.r;
        }
    }
}

void PixelBuffer::blitFrom(const PixelBuffer& src, int sx, int sy, int sw, int sh, int dx, int dy)
{
    assert(src.m_format == m_format);

    if (sw <= 0 || sh <= 0) return;

    int src_x0 = std::max(0, sx);
    int src_y0 = std::max(0, sy);
    int src_x1 = std::min(src.m_width, sx + sw);
    int src_y1 = std::min(src.m_height, sy + sh);

    int width = src_x1 - src_x0;
    int height = src_y1 - src_y0;
    if (width <= 0 || height <= 0) return;

    int dst_x0 = std::max(0, dx);
    int dst_y0 = std::max(0, dy);

    width = std::min(width, m_width - dst_x0);
    height = std::min(height, m_height - dst_y0);
    if (width <= 0 || height <= 0) return;

    for (int i = 0; i < height; i++) 
    {
        const uint8_t* src_row = &src.m_pixels[(src_y0 + i) * src.m_stride + src_x0 * src.m_bpp];

        uint8_t* dst_row = &m_pixels[(dst_y0 + i) * m_stride + dst_x0 * m_bpp];

        memcpy(dst_row, src_row, width * m_bpp);
    }
}

void PixelBuffer::uploadToGLTexture(GLuint tex) const
{
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum formatGL = (m_format == PixelFormat::RGBA8) ? GL_RGBA : GL_RED;
    GLenum internalFmt = (m_format == PixelFormat::RGBA8) ? GL_RGBA8 : GL_R8;

    glTexImage2D(GL_TEXTURE_2D,
        0,
        internalFmt,
        m_width, m_height,
        0,
        formatGL,
        GL_UNSIGNED_BYTE,
        m_pixels.data()
    );
}

uint8_t* PixelBuffer::raw() 
{
    return m_pixels.data();
}

const uint8_t* PixelBuffer::raw() const 
{
    return m_pixels.data();
}