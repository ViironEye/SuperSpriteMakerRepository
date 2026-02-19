#include "SelectionOverlay.h"
#include "../../imgui/imgui.h"
#include <algorithm>
#include <cmath>

static inline ImDrawList* DL() { return ImGui::GetWindowDrawList(); }

static void pushClip(const Viewport& vp)
{
    DL()->PushClipRect(
        ImVec2(vp.originX, vp.originY),
        ImVec2(vp.originX + vp.viewW, vp.originY + vp.viewH),
        true
    );
}
static void popClip() { DL()->PopClipRect(); }

static void rectCanvasToScreen(const Viewport& vp,
    int x, int y, int w, int h,
    ImVec2& outMin, ImVec2& outMax)
{
    float sx0, sy0, sx1, sy1;
    vp.canvasToScreen((float)x, (float)y, sx0, sy0);
    vp.canvasToScreen((float)(x + w), (float)(y + h), sx1, sy1);
    outMin = ImVec2(std::round(sx0), std::round(sy0));
    outMax = ImVec2(std::round(sx1), std::round(sy1));
}

void SelectionOverlay::drawRubberBand(const Viewport& vp,
    int ax, int ay, int bx, int by,
    SelectionOp op)
{
    int x0 = std::min(ax, bx);
    int y0 = std::min(ay, by);
    int x1 = std::max(ax, bx);
    int y1 = std::max(ay, by);

    int w = x1 - x0 + 1;
    int h = y1 - y0 + 1;

    pushClip(vp);

    ImVec2 p0, p1;
    rectCanvasToScreen(vp, x0, y0, w, h, p0, p1);

    // ÷вет зависит от операции (как в редакторах: add/subtract отличаютс€)
    ImU32 fill;
    ImU32 outlineOuter = IM_COL32(0, 0, 0, 220);
    ImU32 outlineInner = IM_COL32(255, 255, 255, 220);

    if (op == SelectionOp::Add)      fill = IM_COL32(80, 160, 255, 50);
    else if (op == SelectionOp::Subtract) fill = IM_COL32(255, 80, 80, 50);
    else                            fill = IM_COL32(160, 160, 160, 40);

    // ѕолупрозрачна€ заливка
    DL()->AddRectFilled(p0, p1, fill);

    // ƒвойной контур (видно на любом фоне)
    DL()->AddRect(p0, p1, outlineOuter, 0.0f, 0, 2.0f);
    DL()->AddRect(p0, p1, outlineInner, 0.0f, 0, 1.0f);

    popClip();
}
