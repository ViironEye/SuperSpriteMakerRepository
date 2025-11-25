#include "PixelBuffer.h"
#include <cstring>
#include <cassert>

PixelBuffer::PixelBuffer(int width, int height, PixelFormat fmt)
    : m_width(width)
    , m_height(height)
    , m_format(fmt)
{
    m_bpp = bytesPerPixel(fmt);
    m_stride = m_width * m_bpp;
    m_pixels.resize(m_stride * m_height);
}

int PixelBuffer::bytesPerPixel(PixelFormat f) const {
    switch (f) {
        case PixelFormat::RGBA8: return 4;
        case PixelFormat::GRAYSCALE8: return 1;
        case PixelFormat::INDEXED8: return 1;
    }
    return 4;
}

void PixelBuffer::clear(const PixelRGBA8& c) {
    if (m_format == PixelFormat::RGBA8) {
        for (int y = 0; y < m_height; y++) {
            PixelRGBA8* row = reinterpret_cast<PixelRGBA8*>(
                &m_pixels[y * m_stride]
                );
            for (int x = 0; x < m_width; x++)
                row[x] = c;
        }
    }
    else {
        uint8_t value = c.r;
        std::fill(m_pixels.begin(), m_pixels.end(), value);
    }
}

void PixelBuffer::setPixel(int x, int y, const PixelRGBA8& c) {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height)
        return;

    uint8_t* row = &m_pixels[y * m_stride];

    if (m_format == PixelFormat::RGBA8) {
        PixelRGBA8* px = reinterpret_cast<PixelRGBA8*>(row + x * 4);
        *px = c;
    }
    else {
        row[x] = c.r;
    }
}


PixelRGBA8 PixelBuffer::getPixel(int x, int y) const {
    assert(x >= 0 && y >= 0 && x < m_width && y < m_height);

    const uint8_t* row = &m_pixels[y * m_stride];

    if (m_format == PixelFormat::RGBA8) {
        const PixelRGBA8* px = reinterpret_cast<const PixelRGBA8*>(row + x * 4);
        return *px;
    }
    else {
        uint8_t v = row[x];
        return PixelRGBA8(v, v, v, 255);
    }
}

uint8_t* PixelBuffer::raw() {
    return m_pixels.data();
}

const uint8_t* PixelBuffer::raw() const {
    return m_pixels.data();
}
