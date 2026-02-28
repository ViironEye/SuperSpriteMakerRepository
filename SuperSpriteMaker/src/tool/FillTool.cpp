#include "FillTool.h"
#include "../model/Frame.h"
#include "../model/PixelBuffer.h"
#include "StrokeCommand.h"
#include <vector>
#include <cstdint>
#include <algorithm>

static inline int iabs(int v) { return v < 0 ? -v : v; }

bool FillTool::match(const PixelRGBA8& a, const PixelRGBA8& b, const FillSettings& s)
{
    if (iabs((int)a.r - (int)b.r) > s.tolerance) return false;
    if (iabs((int)a.g - (int)b.g) > s.tolerance) return false;
    if (iabs((int)a.b - (int)b.b) > s.tolerance) return false;

    if (s.includeAlpha) if (iabs((int)a.a - (int)b.a) > s.tolerance) return false;

    return true;
}

void FillTool::apply(Frame* frame, StrokeCommand* cmd, int x, int y, float /*pressure*/)
{
    if (!frame || !cmd) return;

    PixelBuffer& pb = frame->pixels();
    if (!pb.inBounds(x, y)) return;

    if (pb.format() != PixelFormat::RGBA8) return;

    const PixelRGBA8 target = pb.getPixel(x, y);
    const PixelRGBA8 fill = m_color;

    if (target.r == fill.r && target.g == fill.g && target.b == fill.b && target.a == fill.a) return;

    const int W = pb.width();
    const int H = pb.height();

    if (!m_settings.contiguous)
    {
        for (int yy = 0; yy < H; ++yy)
        {
            for (int xx = 0; xx < W; ++xx)
            {
                PixelRGBA8 cur = pb.getPixel(xx, yy);
                if (!match(cur, target, m_settings)) continue;

                cmd->recordPixel(xx, yy, cur, fill);
                pb.setPixel(xx, yy, fill);
            }
        }
        return;
    }

    std::vector<uint8_t> visited(size_t(W) * size_t(H), 0);

    auto idx = [&](int px, int py) -> size_t { return size_t(py) * size_t(W) + size_t(px); };

    std::vector<int> stack;
    stack.reserve(1024);
    stack.push_back(x);
    stack.push_back(y);

    while (!stack.empty())
    {
        int cy = stack.back(); stack.pop_back();
        int cx = stack.back(); stack.pop_back();

        if (!pb.inBounds(cx, cy)) continue;

        size_t id = idx(cx, cy);
        if (visited[id]) continue;
        visited[id] = 1;

        PixelRGBA8 cur = pb.getPixel(cx, cy);
        if (!match(cur, target, m_settings)) continue;

        cmd->recordPixel(cx, cy, cur, fill);
        pb.setPixel(cx, cy, fill);

        stack.push_back(cx - 1); stack.push_back(cy);
        stack.push_back(cx + 1); stack.push_back(cy);
        stack.push_back(cx);     stack.push_back(cy - 1);
        stack.push_back(cx);     stack.push_back(cy + 1);
    }
}