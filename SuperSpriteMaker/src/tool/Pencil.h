#pragma once
#include "Tool.h"

class PencilTool : public Tool {
public:
    explicit PencilTool(const PixelRGBA8& color);

    void setColor(const PixelRGBA8& c);

    void apply(Frame* frame, StrokeCommand* cmd, BrushRuntimeState& state, int x, int y, float pressure) override;

private:
    PixelRGBA8 m_color;
};
