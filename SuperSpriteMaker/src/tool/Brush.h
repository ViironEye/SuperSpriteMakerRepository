#pragma once
#include <cmath>
#include <algorithm>
#include "../structs/BrushSettings.h"
#include "../structs/BrushRuntimeState.h"
#include "../structs/BrushBlendMode.h"
#include "Tool.h"

class BrushTool : public Tool {
public:
    BrushTool(const PixelRGBA8& color, const BrushSettings& settings);
    explicit BrushTool(const BrushSettings& settings, BrushBlendMode mode = BrushBlendMode::Normal);

    void beginStroke()
    {
        m_state = BrushRuntimeState{};
    }

    void setColor(const PixelRGBA8& c) { m_color = c; }

    void apply(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure) override;

    const BrushSettings& settings() const { return m_settings; }

    void setEraser(bool b) { m_mode = b ? BrushBlendMode::Erase : BrushBlendMode::Normal; }
    bool isEraser() const { return BrushBlendMode::Erase == m_mode; }

private:
    PixelRGBA8 m_color{ 0,0,0,255 };
    BrushSettings m_settings;
    BrushRuntimeState m_state;
    BrushBlendMode m_mode;

    void stamp(Frame* frame, StrokeCommand* cmd, int cx, int cy, float pressure);

    static PixelRGBA8 blend(const PixelRGBA8& dst, const PixelRGBA8& src, uint8_t alpha);
};