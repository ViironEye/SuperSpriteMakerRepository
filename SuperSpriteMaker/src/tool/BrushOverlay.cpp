#include "BrushOverlay.h"
#include "../../imgui/imgui.h"
#include <cmath>

static inline ImDrawList* DL() { return ImGui::GetWindowDrawList(); }

static float clamp01(float v)
{
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

static int segmentsForRadius(float rScreen)
{
    int seg = (int)std::lround(2.0f * 3.14159f * rScreen / 8.0f);
    if (seg < 16) seg = 16;
    if (seg > 128) seg = 128;
    return seg;
}

static void drawDualStrokeCircle(float sx, float sy, float rScreen, int segments)
{
    const ImU32 outer = IM_COL32(0, 0, 0, 220);
    const ImU32 inner = IM_COL32(255, 255, 255, 220);

    DL()->AddCircle(ImVec2(sx, sy), rScreen, outer, segments, 2.0f);
    DL()->AddCircle(ImVec2(sx, sy), rScreen, inner, segments, 1.0f);
}

void BrushOverlay::drawBrushCircle(const Viewport& vp,
    int cx, int cy,
    const BrushSettings& bs,
    float pressure,
    bool drawCenter)
{
    pressure = clamp01(pressure);

    float R = (float)bs.radius;
    if (bs.sizePressure)
        R *= pressure;

    if (R <= 0.0f)
        return;

    // screen center (центр пикселя)
    float sx, sy;
    vp.canvasToScreen((float)cx + 0.5f, (float)cy + 0.5f, sx, sy);

    float rOuter = R * vp.zoom;
    if (rOuter < 1.0f) rOuter = 1.0f;

    // hard edge radius
    float hard = clamp01(bs.hardness);
    float rHard = (R * hard) * vp.zoom;

    // Clip внутри viewport
    const ImVec2 clipMin(vp.originX, vp.originY);
    const ImVec2 clipMax(vp.originX + vp.viewW, vp.originY + vp.viewH);
    DL()->PushClipRect(clipMin, clipMax, true);

    // Внешний круг (radius)
    drawDualStrokeCircle(sx, sy, rOuter, segmentsForRadius(rOuter));

    // Внутренний круг (hard edge), рисуем только если есть заметная разница
    if (hard > 0.0f && rHard >= 2.0f && (rOuter - rHard) >= 2.0f)
    {
        // Сделаем hard edge чуть более “легким”, чтобы не спорил с внешним
        const ImU32 outer = IM_COL32(0, 0, 0, 140);
        const ImU32 inner = IM_COL32(255, 255, 255, 140);
        int seg = segmentsForRadius(rHard);

        DL()->AddCircle(ImVec2(sx, sy), rHard, outer, seg, 2.0f);
        DL()->AddCircle(ImVec2(sx, sy), rHard, inner, seg, 1.0f);
    }

    if (drawCenter)
    {
        const ImU32 outer = IM_COL32(0, 0, 0, 220);
        const ImU32 inner = IM_COL32(255, 255, 255, 220);
        const float k = 4.0f;

        DL()->AddLine(ImVec2(sx - k, sy), ImVec2(sx + k, sy), outer, 2.0f);
        DL()->AddLine(ImVec2(sx, sy - k), ImVec2(sx, sy + k), outer, 2.0f);
        DL()->AddLine(ImVec2(sx - k, sy), ImVec2(sx + k, sy), inner, 1.0f);
        DL()->AddLine(ImVec2(sx, sy - k), ImVec2(sx, sy + k), inner, 1.0f);
    }

    DL()->PopClipRect();
}
