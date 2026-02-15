#include "SelectionOverlayUI.h"
#include "SelectionMoveSession.h"
#include "../../imgui/imgui.h"
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
    // Прямоугольник по границам пикселей: [x..x+w], [y..y+h]
    float sx0, sy0, sx1, sy1;
    vp.canvasToScreen((float)x, (float)y, sx0, sy0);
    vp.canvasToScreen((float)(x + w), (float)(y + h), sx1, sy1);
    outMin = ImVec2(sx0, sy0);
    outMax = ImVec2(sx1, sy1);
}

static void drawDualRect(const ImVec2& p0, const ImVec2& p1, ImU32 cOuter, ImU32 cInner)
{
    DL()->AddRect(p0, p1, cOuter, 0.0f, 0, 2.0f);
    DL()->AddRect(p0, p1, cInner, 0.0f, 0, 1.0f);
}

void SelectionOverlayUI::drawSelectionOutline(const Viewport& vp,
    const SelectionMask& sel,
    bool drawMarchingBox)
{
    if (sel.empty())
        return;

    int bx, by, bw, bh;
    if (!sel.bounds(bx, by, bw, bh))
        return;

    pushClip(vp);

    ImVec2 p0, p1;
    rectCanvasToScreen(vp, bx, by, bw, bh, p0, p1);

    // Немного “подсводим” пиксельно, чтобы линия не дрожала на дробных зумах
    // (если zoom целый — будет идеально)
    p0.x = std::round(p0.x); p0.y = std::round(p0.y);
    p1.x = std::round(p1.x); p1.y = std::round(p1.y);

    // Двойная обводка: видно на любом фоне
    const ImU32 outer = IM_COL32(0, 0, 0, 220);
    const ImU32 inner = IM_COL32(255, 255, 255, 220);

    if (!drawMarchingBox)
    {
        drawDualRect(p0, p1, outer, inner);
        popClip();
        return;
    }

    // "Marching ants" на прямоугольнике: рисуем пунктир по периметру.
    // Паттерн зависит от времени (можно передать phase, но здесь используем ImGui::GetTime()).
    float t = (float)ImGui::GetTime();
    float dash = 6.0f;
    float gap = 4.0f;
    float offset = std::fmod(t * 20.0f, dash + gap);

    // В ImGui нет пунктирной прямоугольной примитивы, рисуем 4 стороны пунктиром вручную.
    auto dashedLine = [&](ImVec2 a, ImVec2 b)
        {
            ImVec2 dir(b.x - a.x, b.y - a.y);
            float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            if (len <= 0.0f) return;
            dir.x /= len; dir.y /= len;

            float pos = -offset;
            while (pos < len)
            {
                float s = (pos < 0.0f) ? 0.0f : pos;
                float e = pos + dash;
                if (e < 0.0f) { pos += dash + gap; continue; }
                if (e > len) e = len;

                ImVec2 pS(a.x + dir.x * s, a.y + dir.y * s);
                ImVec2 pE(a.x + dir.x * e, a.y + dir.y * e);

                // Двойная линия (outer+inner)
                DL()->AddLine(pS, pE, outer, 2.0f);
                DL()->AddLine(pS, pE, inner, 1.0f);

                pos += dash + gap;
            }
        };

    // top, right, bottom, left
    dashedLine(ImVec2(p0.x, p0.y), ImVec2(p1.x, p0.y));
    dashedLine(ImVec2(p1.x, p0.y), ImVec2(p1.x, p1.y));
    dashedLine(ImVec2(p1.x, p1.y), ImVec2(p0.x, p1.y));
    dashedLine(ImVec2(p0.x, p1.y), ImVec2(p0.x, p0.y));

    popClip();
}

void SelectionOverlayUI::drawMoveOutline(const Viewport& vp,
    const SelectionMoveSession& move,
    bool drawGhostOriginal)
{
    if (!move.active())
        return;

    // Нужно, чтобы SelectionMoveSession предоставлял bounds и offset.
    // Добавь геттеры (ниже дам список).
    int bx = move.boundsX();
    int by = move.boundsY();
    int bw = move.boundsW();
    int bh = move.boundsH();

    int dx = move.offsetX();
    int dy = move.offsetY();

    pushClip(vp);

    // Ghost (оригинал) — тонкий контур
    if (drawGhostOriginal)
    {
        ImVec2 g0, g1;
        rectCanvasToScreen(vp, bx, by, bw, bh, g0, g1);
        g0.x = std::round(g0.x); g0.y = std::round(g0.y);
        g1.x = std::round(g1.x); g1.y = std::round(g1.y);

        const ImU32 ghost = IM_COL32(255, 255, 255, 80);
        DL()->AddRect(g0, g1, ghost, 0.0f, 0, 1.0f);
    }

    // Moved box — двойная обводка
    {
        ImVec2 p0, p1;
        rectCanvasToScreen(vp, bx + dx, by + dy, bw, bh, p0, p1);
        p0.x = std::round(p0.x); p0.y = std::round(p0.y);
        p1.x = std::round(p1.x); p1.y = std::round(p1.y);

        const ImU32 outer = IM_COL32(0, 0, 0, 220);
        const ImU32 inner = IM_COL32(255, 255, 255, 220);
        drawDualRect(p0, p1, outer, inner);
    }

    popClip();
}
