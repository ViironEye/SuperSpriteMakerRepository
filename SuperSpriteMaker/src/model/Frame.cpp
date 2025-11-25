#include "Frame.h"
#include <algorithm>

Frame::Frame(int width, int height, PixelFormat fmt, int durationMs) : m_pixels(width, height, fmt), m_duration(durationMs) { }

Frame Frame::clone() const {
	Frame f(m_pixels.width(), m_pixels.height(), m_pixels.format(), m_duration);
	std::copy(m_pixels.data(), m_pixels.data() + m_pixels.stride() * m_pixels.height(), f.pixels().data());
	return f;
}

void Frame::resize(int newW, int newH) 
{
	PixelBuffer newBuf(newW, newH, m_pixels.format());

	int copyW = std::min(newW, m_pixels.width());
	int copyH = std::min(newH, m_pixels.height());

	for (int y = 0; y < copyH; ++y) 
	{
		uint8_t* dst = newBuf.data() + y * newBuf.stride();
		uint8_t* src = m_pixels.data() + y * m_pixels.stride();
		std::copy(src, src + copyW * (m_pixels.format() == PixelFormat::RGBA8 ? 4 : 1), dst);
	}

	m_pixels = std::move(newBuf);
}

void Frame::clear(const PixelRGBA8& color) 
{
	m_pixels.clear(color);
}