#pragma once
#include <cmath>
#include "Tool.h"

class Stroke {
public:
	Stroke(Tool* tool, Frame* frame) : m_tool(tool), m_frame(frame) { }

	void begin(int x, int y)
	{
		m_lastX = x;
		m_lastY = y;
		m_active = true;
		m_tool->apply(m_frame, x, y);
	}

	void update(int x, int y)
	{
		if (!m_active) return;

		int dx = x - m_lastX;
		int dy = y - m_lastY;
		int steps = std::max(std::abs(dx), std::abs(dy));

		for (int i = 1; i <= steps; ++i) 
		{
			float t = steps == 0 ? 1.0f : (float)i / (float)steps;
			int ix = (int)std::round(m_lastX + dx * t);
			int iy = (int)std::round(m_lastY + dy * t);
			m_tool->apply(m_frame, ix, iy);
		}

		m_lastX = x;
		m_lastY = y;
	}

	void end()
	{
		m_active = false;
	}

private:
	Tool* m_tool = nullptr;
	Frame* m_frame = nullptr;

	bool m_active = false;
	int m_lastX = 0;
	int m_lastY = 0;
};