#pragma once
#include "Command.h"

class StrokeCommand : public Command {
public:
    StrokeCommand(Frame* frame) : m_frame(frame)
    {
        auto& pb = m_frame->pixels();
        m_w = pb.width();
        m_h = pb.height();
        m_indexMap.assign(size_t(m_w) * size_t(m_h), -1);
    }

    bool empty() const { return m_diffs.empty(); }

    void recordPixel(int x, int y, const PixelRGBA8& before, const PixelRGBA8& after)
    {
        if (before.r == after.r && before.g == after.g && before.b == after.b && before.a == after.a) return;

        if (x < 0 || y < 0 || x >= m_w || y >= m_h) return;

        const int key = y * m_w + x;
        int& slot = m_indexMap[size_t(key)];

        if (slot == -1)
        {
            slot = (int)m_diffs.size();
            m_diffs.push_back({ x, y, before, after });
        }
        else
        {
            m_diffs[size_t(slot)].after = after;
        }
    }

    void undo() override
    {
        PixelBuffer& buf = m_frame->pixels();
        for (const PixelDiff& d : m_diffs) buf.setPixel(d.x, d.y, d.before);
    }

    void redo() override
    {
        PixelBuffer& buf = m_frame->pixels();
        for (const PixelDiff& d : m_diffs) buf.setPixel(d.x, d.y, d.after);
    }

private:
    Frame* m_frame = nullptr;

    int m_w = 0;
    int m_h = 0;

    std::vector<int> m_indexMap;
    std::vector<PixelDiff> m_diffs;
};
