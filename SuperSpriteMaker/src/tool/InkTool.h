#pragma once

#pragma once
#include "Tool.h"
#include "../model/PixelBuffer.h"
#include "../structs/InkSettings.h"

class InkTool : public Tool
{
public:
    explicit InkTool(const PixelRGBA8& color)
        : m_color(color)
    {
    }

    void setColor(const PixelRGBA8& c) { m_color = c; }
    InkSettings& settings() { return m_settings; }
    const InkSettings& settings() const { return m_settings; }

    void apply(Frame* frame,
        StrokeCommand* cmd,
        int x, int y,
        float pressure) override;

private:
    PixelRGBA8 m_color;
    InkSettings m_settings;

    static uint8_t clampU8(int v)
    {
        if (v < 0) return 0;
        if (v > 255) return 255;
        return (uint8_t)v;
    }

    // src-over композитинг одного пикселя (dst = src over dst)
    static PixelRGBA8 blendSrcOver(const PixelRGBA8& dst, const PixelRGBA8& src);
};
