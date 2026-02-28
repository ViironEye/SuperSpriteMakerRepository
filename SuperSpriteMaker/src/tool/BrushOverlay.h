#pragma once
#include "../application/Viewport.h"
#include "../structs/BrushSettings.h"

struct BrushOverlay
{
    static void drawBrushCircle(const Viewport& vp, int cx, int cy, const BrushSettings& bs, float pressure, bool drawCenter = true);
};
