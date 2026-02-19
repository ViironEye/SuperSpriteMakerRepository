#pragma once
#pragma once
#include "SelectionMask.h"
#include "../model/PixelBuffer.h"
#include "../application/Viewport.h"
#include <cstdint>

struct SelectionOverlay {
    // phase увеличивай каждый кадр (0..N) для анимации
    static void drawMarchingAnts(PixelBuffer& dst,
        const SelectionMask& sel,
        int phase)
    {
        if (sel.empty()) return;

        // “муравьи” = рисуем только границу выделения
        // Паттерн: (x+y+phase) % 8 < 4 -> белый, иначе черный
        for (int y = 0; y < sel.height(); ++y) {
            for (int x = 0; x < sel.width(); ++x) {
                if (!sel.get(x, y)) continue;

                // Пиксель на границе, если есть сосед НЕ в выделении
                bool border =
                    !sel.get(x - 1, y) || !sel.get(x + 1, y) ||
                    !sel.get(x, y - 1) || !sel.get(x, y + 1);

                if (!border) continue;
                if (!dst.inBounds(x, y)) continue;

                int t = (x + y + phase) & 7;
                PixelRGBA8 c = (t < 4) ? PixelRGBA8(255, 255, 255, 255)
                    : PixelRGBA8(0, 0, 0, 255);
                dst.setPixel(x, y, c);
            }
        }
    }
    static void drawRubberBand(const Viewport& vp,
        int ax, int ay, int bx, int by,
        SelectionOp op);
};
