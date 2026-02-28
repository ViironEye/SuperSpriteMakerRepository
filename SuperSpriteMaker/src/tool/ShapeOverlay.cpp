#include "ShapeOverlay.h"
#include "../../imgui/imgui.h"
#include <vector>
#include <cmath>
#include <algorithm>

static inline ImDrawList* DL() { return ImGui::GetWindowDrawList(); }

static ImU32 colU32(const PixelRGBA8& c, float alphaMul)
{
    int a = (int)std::round((c.a / 255.0f) * alphaMul * 255.0f);
    if (a < 0) a = 0; if (a > 255) a = 255;
    return IM_COL32(c.r, c.g, c.b, a);
}

static void pushClip(const Viewport& vp)
{
    DL()->PushClipRect( ImVec2(vp.originX, vp.originY), ImVec2(vp.originX + vp.viewW, vp.originY + vp.viewH), true);
}

static void popClip() { DL()->PopClipRect(); }

static void canvasToScreen(const Viewport& vp, float cx, float cy, ImVec2& out)
{
    float sx, sy;
    vp.canvasToScreen(cx, cy, sx, sy);
    out = ImVec2(sx, sy);
}

static void rectToScreen(const Viewport& vp, int x0, int y0, int x1, int y1, ImVec2& p0, ImVec2& p1)
{
    int lx = std::min(x0, x1);
    int rx = std::max(x0, x1);
    int ty = std::min(y0, y1);
    int by = std::max(y0, y1);

    canvasToScreen(vp, (float)lx, (float)ty, p0);
    canvasToScreen(vp, (float)(rx + 1), (float)(by + 1), p1);
}

static int segsForEllipse(float rxScreen, float ryScreen)
{
    float r = std::max(rxScreen, ryScreen);
    int s = (int)std::lround(2.0f * 3.14159f * r / 10.0f);
    if (s < 24) s = 24;
    if (s > 160) s = 160;
    return s;
}

void ShapeOverlay::drawPreview(const Viewport& vp, int x0, int y0, int x1, int y1, const ShapeSettings& s, const PixelRGBA8& color)
{
    pushClip(vp);

    ImU32 fillCol = colU32(color, 0.25f);
    ImU32 outlineCol = colU32(color, 0.90f);

    float outlineTh = std::max(1.0f, (float)s.thickness);

    if (s.mode == ShapeMode::Rectangle)
    {
        ImVec2 p0, p1;
        rectToScreen(vp, x0, y0, x1, y1, p0, p1);

        if (s.filled) DL()->AddRectFilled(p0, p1, fillCol);

        DL()->AddRect(p0, p1, outlineCol, 0.0f, 0, outlineTh);
    }
    else if (s.mode == ShapeMode::Line)
    {
        ImVec2 a, b;
        canvasToScreen(vp, (float)x0 + 0.5f, (float)y0 + 0.5f, a);
        canvasToScreen(vp, (float)x1 + 0.5f, (float)y1 + 0.5f, b);

        DL()->AddLine(a, b, outlineCol, outlineTh);
    }
    else if (s.mode == ShapeMode::Ellipse)
    {
        int lx = std::min(x0, x1), rx = std::max(x0, x1);
        int ty = std::min(y0, y1), by = std::max(y0, y1);

        float cx = (lx + rx + 1) * 0.5f;
        float cy = (ty + by + 1) * 0.5f;
        float rxC = (rx - lx + 1) * 0.5f;
        float ryC = (by - ty + 1) * 0.5f;

        float cxS, cyS, xEdgeS, yEdgeS;
        vp.canvasToScreen(cx, cy, cxS, cyS);
        vp.canvasToScreen(cx + rxC, cy, xEdgeS, yEdgeS);
        float rxS = std::fabs(xEdgeS - cxS);

        vp.canvasToScreen(cx, cy + ryC, xEdgeS, yEdgeS);
        float ryS = std::fabs(yEdgeS - cyS);

        int segs = segsForEllipse(rxS, ryS);

        std::vector<ImVec2> pts;
        pts.reserve((size_t)segs);

        for (int i = 0; i < segs; ++i)
        {
            float t = (float)i / (float)segs;
            float ang = t * 2.0f * 3.14159f;
            float x = cx + std::cos(ang) * rxC;
            float y = cy + std::sin(ang) * ryC;
            ImVec2 p;
            canvasToScreen(vp, x, y, p);
            pts.push_back(p);
        }

        if (s.filled) DL()->AddConvexPolyFilled(pts.data(), (int)pts.size(), fillCol);

        DL()->AddPolyline(pts.data(), (int)pts.size(), outlineCol, true, outlineTh);
    }

    popClip();
}
