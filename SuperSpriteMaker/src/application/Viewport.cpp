#include "Viewport.h"

void Viewport::setViewRect(float x, float y, float w, float h)
{
    originX = x; originY = y; viewW = w; viewH = h;
}

bool Viewport::screenToCanvas(float sx, float sy, int& outX, int& outY) const
{
    // sx/sy — абсолютные координаты окна
    float localX = sx - originX;
    float localY = sy - originY;

    // внутри ли viewport
    if (localX < 0 || localY < 0 || localX >= viewW || localY >= viewH)
        return false;

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

void Viewport::zoomAt(float factor, float screenX, float screenY)
{
    // zoom фокусируется на точке курсора
    float beforeX = (screenX - originX - panX) / zoom;
    float beforeY = (screenY - originY - panY) / zoom;

    zoom *= factor;
    if (zoom < 1.0f) zoom = 1.0f;
    if (zoom > 128.0f) zoom = 128.0f;

    float afterX = (screenX - originX - panX) / zoom;
    float afterY = (screenY - originY - panY) / zoom;

    panX += (afterX - beforeX) * zoom;
    panY += (afterY - beforeY) * zoom;
}