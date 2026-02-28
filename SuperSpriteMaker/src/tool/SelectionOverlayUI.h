#pragma once
#include "../application/Viewport.h"
#include "SelectionMask.h"

class SelectionMoveSession;

struct SelectionOverlayUI
{
    static void drawSelectionOutline(const Viewport& vp, const SelectionMask& sel, bool drawMarchingBox = true);

    static void drawMoveOutline(const Viewport& vp, const SelectionMoveSession& move, bool drawGhostOriginal = true);
};
