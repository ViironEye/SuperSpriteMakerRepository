#pragma once
#pragma once
#include <cmath>

struct Viewport
{
    // экранные координаты (в пикселях окна) -> canvas координаты (в пикселях спрайта)
    float panX = 0.0f;   // сдвиг в экране
    float panY = 0.0f;
    float zoom = 16.0f;  // сколько экранных пикселей на 1 canvas пиксель (16x удобно)

    // Позиция области рисования в окне UI (левый верх) и её размер
    float originX = 0.0f;
    float originY = 0.0f;
    float viewW = 0.0f;
    float viewH = 0.0f;

    void setViewRect(float x, float y, float w, float h);

    // Screen -> Canvas (int pixels)
    bool screenToCanvas(float sx, float sy, int& outX, int& outY) const;

    // Canvas -> Screen (float)
    void canvasToScreen(float cx, float cy, float& outSX, float& outSY) const;

    void zoomAt(float factor, float screenX, float screenY);
};
