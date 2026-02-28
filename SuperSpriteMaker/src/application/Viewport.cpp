#include "Viewport.h"
#include <algorithm>
#include <cmath>

bool Viewport::screenToCanvas(float sx, float sy, int& outX, int& outY) const
{
    float localX = sx - originX;
    float localY = sy - originY;

    if (localX < 0 || localY < 0 || localX >= viewW || localY >= viewH) return false;

    float cx = (localX - panX) / zoom;
    float cy = (localY - panY) / zoom;

    outX = (int)std::floor(cx);
    outY = (int)std::floor(cy);
    return true;
}

void Viewport::canvasToScreen(float cx, float cy, float& outSX, float& outSY) const
{
    outSX = originX + panX + cx * zoom;
    outSY = originY + panY + cy * zoom;
}

static inline float clampf(float v, float a, float b) { return std::max(a, std::min(v, b)); }

void Viewport::zoomAt(float factor, float screenX, float screenY)
{
    float oldZoom = zoom;
    float newZoom = clampf(oldZoom * factor, zoomMin, zoomMax);
    if (newZoom == oldZoom) return;

    float cx = (screenX - originX - panX) / oldZoom;
    float cy = (screenY - originY - panY) / oldZoom;

    zoom = newZoom;

    panX = screenX - originX - cx * zoom;
    panY = screenY - originY - cy * zoom;

    clampPan();
}

void Viewport::fitToView()
{
    if (canvasW <= 0 || canvasH <= 0) return;

    float margin = 8.0f;

    float zx = (viewW - margin * 2.0f) / (float)canvasW;
    float zy = (viewH - margin * 2.0f) / (float)canvasH;
    float z = std::min(zx, zy);

    zoom = clampf(z, zoomMin, zoomMax);

    float canvasPixW = canvasW * zoom;
    float canvasPixH = canvasH * zoom;

    panX = (viewW - canvasPixW) * 0.5f;
    panY = (viewH - canvasPixH) * 0.5f;

    clampPan();
}

void Viewport::clampPan()
{
    if (canvasW <= 0 || canvasH <= 0) return;

    float canvasPixW = canvasW * zoom;
    float canvasPixH = canvasH * zoom;
    float keep = 32.0f;

    float minX, maxX, minY, maxY;

    if (canvasPixW <= viewW)
    {
        float cx = (viewW - canvasPixW) * 0.5f;
        minX = maxX = cx;
    }
    else
    {
        minX = viewW - canvasPixW - keep;
        maxX = keep;
    }

    if (canvasPixH <= viewH)
    {
        float cy = (viewH - canvasPixH) * 0.5f;
        minY = maxY = cy;
    }
    else
    {
        minY = viewH - canvasPixH - keep;
        maxY = keep;
    }

    panX = clampf(panX, minX, maxX);
    panY = clampf(panY, minY, maxY);
}