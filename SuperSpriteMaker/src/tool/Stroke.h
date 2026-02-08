#pragma once
#include <cmath>
#include "Tool.h"
#include "StrokeCommand.h"

class Stroke
{
public:
    Stroke(Tool* tool, Frame* frame, StrokeCommand* cmd)
        : m_tool(tool)
        , m_frame(frame)
        , m_cmd(cmd)
    {
    }

    void begin(int x, int y, float pressure)
    {
        m_active = true;
        m_lastX = x;
        m_lastY = y;

        // первая точка
        m_tool->apply(m_frame, m_cmd, x, y, pressure);
    }

    void update(int x, int y, float pressure)
    {
        if (!m_active)
            return;

        int dx = x - m_lastX;
        int dy = y - m_lastY;

        int steps = std::max(std::abs(dx), std::abs(dy));
        if (steps == 0)
            return;

        for (int i = 1; i <= steps; ++i)
        {
            float t = float(i) / float(steps);
            int ix = int(std::round(m_lastX + dx * t));
            int iy = int(std::round(m_lastY + dy * t));

            m_tool->apply(m_frame, m_cmd, ix, iy, pressure);
        }

        m_lastX = x;
        m_lastY = y;
    }

    void end()
    {
        m_active = false;
    }

private:
    Tool* m_tool = nullptr;               // не владеет
    Frame* m_frame = nullptr;             // не владеет
    StrokeCommand* m_cmd = nullptr;       // не владеет

    bool m_active = false;
    int m_lastX = 0;
    int m_lastY = 0;
};
