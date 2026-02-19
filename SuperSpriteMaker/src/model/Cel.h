#pragma once
#include "Frame.h"
#include "../structs/PixelFormatStruct.h"

class Cel
{
public:
    Cel(int width, int height, PixelFormat fmt, int durationMs, int x = 0, int y = 0)
        : m_frame(width, height, fmt, durationMs), m_x(x), m_y(y) {
    }

    Frame* frame() { return &m_frame; }
    const Frame* frame() const { return &m_frame; }

    PixelBuffer& pixels() { return m_frame.pixels(); }
    const PixelBuffer& pixels() const { return m_frame.pixels(); }

    int x() const { return m_x; }
    int y() const { return m_y; }

    void setX(int v) { m_x = v; }
    void setY(int v) { m_y = v; }

private:
    Frame m_frame;
    int m_x = 0;
    int m_y = 0;
};