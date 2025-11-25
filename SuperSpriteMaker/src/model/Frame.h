#pragma once
#include "PixelBuffer.h"
#include <string>
#include <cstdint>

class Frame {
public:
	Frame(int width, int height, PixelFormat fmt, int durationMs = 100);

	Frame clone() const;

	PixelBuffer& pixels() { return m_pixels; }
	const PixelBuffer& pixels() const { return m_pixels; }

	int duration() const { return m_duration; }
	void setDuration(int ms) { m_duration = ms; }

	void resize(int newW, int newH);

	void clear(const PixelRGBA8& color);

private:
	PixelBuffer m_pixels;

	//duration in milliseconds
	int m_duration;
};