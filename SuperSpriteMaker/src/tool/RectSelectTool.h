#pragma once
#pragma once
#include "SelectionMask.h"

class RectSelectTool {
public:
    void begin(int x, int y, SelectionOp op) 
    { 
        m_ax = m_bx = x; 
        m_ay = m_by = y; 
        m_hasSelection = true; 
        m_op = op; 
    }

    void update(int x, int y) { m_bx = x; m_by = y; }

    void end(SelectionMask& sel)
    {
        if (!m_hasSelection) return;
        m_hasSelection = false;

        int x0 = std::min(m_ax, m_bx);
        int y0 = std::min(m_ay, m_by);
        int x1 = std::max(m_ax, m_bx);
        int y1 = std::max(m_ay, m_by);

        sel.applyRect(x0, y0, x1 - x0 + 1, y1 - y0 + 1, m_op);
    }

    void clear()
    {
        m_ax = 0;
        m_ay = 0;
        m_bx = 0;
        m_by = 0;
        m_hasSelection = false;
    }

    bool active() const { return m_hasSelection; }
    int ax() const { return m_ax; }
    int ay() const { return m_ay; }
    int bx() const { return m_bx; }
    int by() const { return m_by; }
    SelectionOp op() const { return m_op; }

private:
    bool m_hasSelection = false;
    int m_ax = 0, m_ay = 0, m_bx = 0, m_by = 0;
    SelectionOp m_op = SelectionOp::Replace;
};
