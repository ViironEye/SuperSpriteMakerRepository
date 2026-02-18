#include "GLTexturePresenter.h"
#include "Viewport.h"
#include "../model/PixelBuffer.h"
#include "../../imgui/imgui.h"

static inline ImDrawList* DL() { return ImGui::GetWindowDrawList(); }

bool GLTexturePresenter::present(const PixelBuffer& pb,
    const Viewport& vp,
    const PresenterOptions& opt)
{
    const int imgW = pb.width();
    const int imgH = pb.height();
    if (imgW <= 0 || imgH <= 0)
        return false;

    m_cache.upload(pb);

    const ImVec2 clipMin(vp.originX, vp.originY);
    const ImVec2 clipMax(vp.originX + vp.viewW, vp.originY + vp.viewH);
    DL()->PushClipRect(clipMin, clipMax, true);

    const float x0 = vp.originX + vp.panX;
    const float y0 = vp.originY + vp.panY;
    const float x1 = x0 + imgW * vp.zoom;
    const float y1 = y0 + imgH * vp.zoom;

    if (opt.checkerboard)
        drawCheckerboard(vp, imgW, imgH, opt.checkerCellCanvasPx);

    const ImVec2 pMin(x0, y0);
    const ImVec2 pMax(x1, y1);

    const ImVec2 uv0(0.0f, opt.flipY ? 1.0f : 0.0f);
    const ImVec2 uv1(1.0f, opt.flipY ? 0.0f : 1.0f);

    DL()->AddImage(m_cache.imguiID(), pMin, pMax, uv0, uv1);

    if (opt.outline)
        drawOutline(vp, imgW, imgH);

    if (opt.grid && (int)vp.zoom >= opt.gridMinZoom)
        drawGrid(vp, imgW, imgH);

    DL()->PopClipRect();
    return true;
}

void GLTexturePresenter::drawCheckerboard(const Viewport& vp, int imgW, int imgH, int cellCanvasPx) const
{
    if (cellCanvasPx <= 0) cellCanvasPx = 8;

    const float cell = float(cellCanvasPx) * vp.zoom;

    const float x0 = vp.originX + vp.panX;
    const float y0 = vp.originY + vp.panY;
    const float x1 = x0 + imgW * vp.zoom;
    const float y1 = y0 + imgH * vp.zoom;

    // Нейтральные серые (UI-элемент)
    const ImU32 c0 = IM_COL32(90, 90, 90, 255);
    const ImU32 c1 = IM_COL32(120, 120, 120, 255);

    for (float y = y0; y < y1; y += cell) {
        for (float x = x0; x < x1; x += cell) {
            const int ix = int((x - x0) / cell);
            const int iy = int((y - y0) / cell);
            const ImU32 c = ((ix + iy) & 1) ? c0 : c1;

            float rx1 = x + cell; if (rx1 > x1) rx1 = x1;
            float ry1 = y + cell; if (ry1 > y1) ry1 = y1;

            DL()->AddRectFilled(ImVec2(x, y), ImVec2(rx1, ry1), c);
        }
    }
}

void GLTexturePresenter::drawGrid(const Viewport& vp, int imgW, int imgH) const
{
    const float x0 = vp.originX + vp.panX;
    const float y0 = vp.originY + vp.panY;
    const float x1 = x0 + imgW * vp.zoom;
    const float y1 = y0 + imgH * vp.zoom;

    // тонкая сетка (полупрозрачная)
    const ImU32 gridC = IM_COL32(0, 0, 0, 60);

    for (int x = 0; x <= imgW; ++x) {
        float sx = x0 + x * vp.zoom;
        DL()->AddLine(ImVec2(sx, y0), ImVec2(sx, y1), gridC);
    }
    for (int y = 0; y <= imgH; ++y) {
        float sy = y0 + y * vp.zoom;
        DL()->AddLine(ImVec2(x0, sy), ImVec2(x1, sy), gridC);
    }
}

void GLTexturePresenter::drawOutline(const Viewport& vp, int imgW, int imgH) const
{
    const float x0 = vp.originX + vp.panX;
    const float y0 = vp.originY + vp.panY;
    const float x1 = x0 + imgW * vp.zoom;
    const float y1 = y0 + imgH * vp.zoom;

    const ImU32 border = IM_COL32(0, 0, 0, 140);
    DL()->AddRect(ImVec2(x0, y0), ImVec2(x1, y1), border);
}

