#pragma once
#include <cmath>
#include "Tool.h"

class Stroke {
public:
    Stroke(Tool* tool, Frame* frame, StrokeCommand* cmd) : m_tool(tool), m_frame(frame), m_cmd(cmd) { }

    void begin(int x, int y);
    void update(int x, int y);
    void end();

private:
    Tool* m_tool = nullptr;
    Frame* m_frame = nullptr;
    StrokeCommand* m_cmd = nullptr;

    int m_lastX = 0;
    int m_lastY = 0;
    bool m_active = false;
};