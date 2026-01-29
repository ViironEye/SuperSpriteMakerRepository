#include "Stroke.h"

void Stroke::begin(int x, int y)
{
    m_lastX = x;
    m_lastY = y;
    m_active = true;

    m_tool->apply(m_frame, m_cmd, x, y);
}

void Stroke::update(int x, int y)
{
    if (!m_active) return;

    int dx = x - m_lastX;
    int dy = y - m_lastY;
    int steps = std::max(std::abs(dx), std::abs(dy));

    for (int i = 1; i <= steps; ++i) 
    {
        float t = steps == 0 ? 1.0f : float(i) / float(steps);

        int ix = int(std::round(m_lastX + dx * t));
        int iy = int(std::round(m_lastY + dy * t));

        m_tool->apply(m_frame, m_cmd, ix, iy);
    }

    m_lastX = x;
    m_lastY = y;
}

void Stroke::end()
{
    m_active = false;
}