#pragma once
#include "Brush.h"

BrushTool::BrushTool(const PixelRGBA8& color, const BrushSettings& settings) : m_color(color), m_settings(settings) { }

void BrushTool::apply(Frame* frame, BrushRuntimeState& state, int x, int y, float pressure)
{
    if (!frame) return;

    if (state.first)
    {
        stamp(frame, x, y, pressure);
        state.lastX = (float)x;
        state.lastY = (float)y;
        state.accumulatedDistance = 0.0f;
        state.first = false;
        return;
    }

    float dx = x - state.lastX;
    float dy = y - state.lastY;
    float dist = std::sqrt(dx * dx + dy * dy);

    state.accumulatedDistance += dist;

    float R = (float)m_settings.radius;
    if (m_settings.sizePressure)
    {
        R *= pressure;
    }

    float spacingPx = std::max(1.0f, R * m_settings.spacing);

    float nx = dx / dist;
    float ny = dy / dist;

    while (state.accumulatedDistance >= spacingPx)
    {
        state.lastX += nx * spacingPx;
        state.lastY += ny * spacingPx;

        stamp(frame, (int)std::round(state.lastX), (int)std::round(state.lastY), pressure);

        state.accumulatedDistance -= spacingPx;
    }
}

void BrushTool::stamp(Frame* frame, int cx, int cy, float pressure)
{
    PixelBuffer& pb = frame->pixels();

    float R = m_settings.radius;

    if (m_settings.sizePressure)
    {
        R *= pressure;
    }

    int ir = (int)std::ceil(R);

    for (int y = -ir; y <= ir; ++y)
    {
        for (int x = -ir; x <= ir; ++x)
        {
            int px = cx + x;
            int py = cy + y;

            if (!pb.inBounds(px, py)) continue;

            float d = std::sqrt(float(x * x + y * y));

            if (d > R) continue;

            float hardR = m_settings.hardness * R;
            float alphaFactor;

            if (d <= hardR)
            {
                alphaFactor = 1.0f;
            }
            else
            {
                alphaFactor = 1.0f - (d - hardR) / (R - hardR);
            }

            float alpha = m_settings.opacity * alphaFactor;

            if (m_settings.opacityPressure) alpha *= pressure;

            PixelRGBA8 src = m_color;
            src.a = uint8_t(src.a * alpha);

            pb.setPixel(px, py, src);
        }
    }
}

    PixelRGBA8 BrushTool::blend(const PixelRGBA8& dst, const PixelRGBA8& src, uint8_t alpha)
    {
        float a = alpha / 255.0f;
        float ia = 1.0f - a;

        return PixelRGBA8(
            uint8_t(dst.r * ia + src.r * a),
            uint8_t(dst.g * ia + src.g * a),
            uint8_t(dst.b * ia + src.b * a),
            255
        );
    }