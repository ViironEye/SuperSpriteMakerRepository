#pragma once
#include <cmath>
#include "Tool.h"
#include "StrokeCommand.h"
#include "ShapeTool.h"

class Stroke
{
    public:
        Stroke(Tool* tool, Frame* frame, StrokeCommand* cmd) : m_tool(tool), m_frame(frame), m_cmd(cmd) { }

        void begin(int x, int y, float pressure)
        {
            if (auto* shape = dynamic_cast<ShapeTool*>(m_tool))
                shape->begin(m_frame, m_cmd, x, y, pressure);
            else
                m_tool->apply(m_frame, m_cmd, x, y, pressure);
        }

        void update(int x, int y, float pressure)
        {
            if (auto* shape = dynamic_cast<ShapeTool*>(m_tool))
                shape->update(m_frame, m_cmd, x, y, pressure);
            else
                m_tool->apply(m_frame, m_cmd, x, y, pressure);
        }

        void end(int x, int y, float pressure)
        {
            if (auto* shape = dynamic_cast<ShapeTool*>(m_tool))
                shape->end(m_frame, m_cmd, x, y, pressure);
        }
private:
    Tool* m_tool = nullptr;
    Frame* m_frame = nullptr;
    StrokeCommand* m_cmd = nullptr;

    bool m_active = false;
    int m_lastX = 0;
    int m_lastY = 0;
};
