#pragma once
#pragma once
#include "Tool.h"
#include "../structs/ShapeSettings.h"
#include <optional>

class ShapeTool : public Tool
{
public:
    ShapeTool(const ShapeSettings& s) : m_settings(s) { }

    void setSettings(const ShapeSettings& s) { m_settings = s; }
    const ShapeSettings& settings() const { return m_settings; }

    void begin(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure);
    void update(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure);
    void end(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure);

    void apply(Frame*, StrokeCommand*, int, int, float) override {}
    void setColor(const PixelRGBA8& c) { m_color = c; }

    bool isActive() const { return m_active; }
    int  x0() const { return m_x0; }
    int  y0() const { return m_y0; }
    int  x1() const { return m_x1; }
    int  y1() const { return m_y1; }

    PixelRGBA8 color() const { return m_color; }

private:
    void rasterize(Frame* frame, StrokeCommand* cmd);

    void drawRect(Frame* frame, StrokeCommand* cmd);
    void drawLine(Frame* frame, StrokeCommand* cmd);
    void drawEllipse(Frame* frame, StrokeCommand* cmd);

    ShapeSettings m_settings;

    bool m_active = false;
    int m_x0 = 0, m_y0 = 0;
    int m_x1 = 0, m_y1 = 0;

    PixelRGBA8 m_color = PixelRGBA8(255, 255, 255, 255);
};
