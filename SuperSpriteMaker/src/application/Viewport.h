#pragma once

struct Viewport
{
    float panX = 0.0f;
    float panY = 0.0f;
    float zoom = 16.0f;

    float originX = 0.0f;
    float originY = 0.0f;
    float viewW = 1.0f;
    float viewH = 1.0f;

    float zoomMin = 0.25f;
    float zoomMax = 128.0f;

    int canvasW = 0;
    int canvasH = 0;

    bool screenToCanvas(float sx, float sy, int& outX, int& outY) const;
    void setViewRect(float x, float y, float w, float h) { originX = x; originY = y; viewW = w; viewH = h; }

    void setCanvasSize(int w, int h) { canvasW = w; canvasH = h; }

    void fitToView();
    void clampPan();
    void canvasToScreen(float cx, float cy, float& outSX, float& outSY) const;

    void zoomAt(float factor, float screenX, float screenY);
};
