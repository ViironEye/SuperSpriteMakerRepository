#pragma once
#include "../application/Viewport.h"
#include "../structs/ShapeSettings.h"
#include "../model/PixelBuffer.h"

struct ShapeOverlay
{
    static void drawPreview(const Viewport& vp,
        int x0, int y0, int x1, int y1,
        const ShapeSettings& s,
        const PixelRGBA8& color);
};
