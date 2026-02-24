#pragma once
#include "Tool.h"
#include "../structs/PixelFormatStruct.h"

struct FillSettings
{
    int  tolerance = 0;
    bool contiguous = true;
    bool includeAlpha = true;
};

class FillTool : public Tool
{
public:
    FillTool() = default;
    explicit FillTool(const PixelRGBA8& c) : m_color(c) {}

    void setColor(const PixelRGBA8& c) { m_color = c; }
    PixelRGBA8 color() const { return m_color; }

    FillSettings& settings() { return m_settings; }
    const FillSettings& settings() const { return m_settings; }
    void setSettings(const FillSettings& s) { m_settings = s; }

    void apply(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure) override;

private:
    static bool match(const PixelRGBA8& a, const PixelRGBA8& b, const FillSettings& s);

    PixelRGBA8 m_color{ 0,0,0,255 };
    FillSettings m_settings;
};