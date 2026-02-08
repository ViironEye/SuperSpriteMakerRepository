#pragma once

#pragma once
#include <cstdint>
#include <cmath>
#include "../structs/PixelFormatStruct.h"

enum class BlendMode {
    Normal,
    Multiply,
    Screen,
    Add,
    Subtract,
    Darken,
    Lighten
};

struct Blend {
    // Все операции предполагают НЕ premultiplied хранение в PixelBuffer (как у тебя сейчас).
    // srcOpacity: 0..255 — дополнительная непрозрачность слоя/кисти (layer opacity).
    static PixelRGBA8 apply(BlendMode mode,
        const PixelRGBA8& dst,
        const PixelRGBA8& src,
        uint8_t srcOpacity = 255);

private:
    static uint8_t clampU8(int v) {
        if (v < 0) return 0;
        if (v > 255) return 255;
        return (uint8_t)v;
    }

    static float u8To01(uint8_t v) { return v / 255.0f; }
    static uint8_t f01ToU8(float v) {
        if (v < 0.0f) v = 0.0f;
        if (v > 1.0f) v = 1.0f;
        return (uint8_t)std::lround(v * 255.0f);
    }

    // Базовые channel-blend функции (в 0..1)
    static float blendChannel(BlendMode mode, float d, float s);

    // Общая формула композитинга: out = B(d,s) смешанный с dst по srcAlpha
    // с учетом srcOpacity.
    static PixelRGBA8 composite(const PixelRGBA8& dst,
        const PixelRGBA8& src,
        uint8_t srcOpacity,
        BlendMode mode);
};
