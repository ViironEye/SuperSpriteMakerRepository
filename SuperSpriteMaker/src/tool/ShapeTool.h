#pragma once
#pragma once
#include "Tool.h"
#include "../structs/ShapeSettings.h"
#include <optional>

class ShapeTool : public Tool
{
public:
    ShapeTool(const ShapeSettings& s) : m_settings(s) {}

    void setSettings(const ShapeSettings& s) { m_settings = s; }
    const ShapeSettings& settings() const { return m_settings; }

    // Stroke lifecycle
    void begin(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure);
    void update(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure);
    void end(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure);

    // Tool::apply не используем для shapes
    void apply(Frame*, StrokeCommand*, int, int, float) override {}

private:
    void rasterize(Frame* frame, StrokeCommand* cmd);

    void drawRect(Frame* frame, StrokeCommand* cmd);
    void drawLine(Frame* frame, StrokeCommand* cmd);
    void drawEllipse(Frame* frame, StrokeCommand* cmd);

private:
    ShapeSettings m_settings;

    bool m_active = false;
    int m_x0 = 0, m_y0 = 0;
    int m_x1 = 0, m_y1 = 0;

    PixelRGBA8 m_color = PixelRGBA8(255, 255, 255, 255); // позже брать из палитры
};
