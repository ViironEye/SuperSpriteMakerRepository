#pragma once
#include <cmath>
#include "../structs/BrushSettings.h"
#include "../structs/BrushRuntimeState.h"
#include "Tool.h"

class BrushTool : public Tool {
public:
    BrushTool(const PixelRGBA8& color, const BrushSettings& settings);

    void beginStroke()
    {
        m_state = BrushRuntimeState{};
    }

    void apply(Frame* frame, BrushRuntimeState& state, int x, int y, float pressure);

private:
    PixelRGBA8 m_color;
    BrushSettings m_settings;
    BrushRuntimeState m_state;

    void stamp(Frame* frame, int cx, int cy, float pressure);
    static PixelRGBA8 blend(const PixelRGBA8& dst, const PixelRGBA8& src, uint8_t alpha);
};