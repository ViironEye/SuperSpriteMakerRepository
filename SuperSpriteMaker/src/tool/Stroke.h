#pragma once
#include <cmath>
#include "Brush.h"

class Stroke
{
public:
    void begin(Frame* frame, BrushTool* tool, int x, int y, float pressure)
    {
        m_frame = frame;
        m_tool = tool;
        m_state = {};
        m_tool->apply(frame, m_state, x, y, pressure);
    }

    void update(int x, int y, float pressure)
    {
        m_tool->apply(m_frame, m_state, x, y, pressure);
    }

private:
    Frame* m_frame = nullptr;
    BrushTool* m_tool = nullptr;
    BrushRuntimeState m_state;
};
