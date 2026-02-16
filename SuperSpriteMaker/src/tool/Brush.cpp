#pragma once
#include "Brush.h"

BrushTool::BrushTool(const PixelRGBA8& color, const BrushSettings& settings) : m_color(color), m_settings(settings) { }

BrushTool::BrushTool(const BrushSettings& settings, BrushBlendMode mode) : m_settings(settings), m_mode(mode) { }

/*void BrushTool::apply(Frame* frame, BrushRuntimeState& state, int x, int y, float pressure)
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
} */
void BrushTool::apply(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure)
{
    BrushRuntimeState state = m_state;

    if (!frame || !cmd) return;

    if (state.first)
    {
        stamp(frame, cmd, x, y, pressure);
        state.lastX = float(x);
        state.lastY = float(y);
        state.accumulatedDistance = 0.0f;
        state.first = false;
        return;
    }

    float dx = float(x) - state.lastX;
    float dy = float(y) - state.lastY;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist <= 0.0001f) return;

    float radius = float(m_settings.radius);
    if (m_settings.sizePressure) radius *= pressure;

    float spacingPx = radius * m_settings.spacing;
    if (spacingPx < 1.0f) spacingPx = 1.0f;

    state.accumulatedDistance += dist;

    float nx = dx / dist;
    float ny = dy / dist;

    while (state.accumulatedDistance >= spacingPx)
    {
        state.lastX += nx * spacingPx;
        state.lastY += ny * spacingPx;

        stamp(frame, cmd, int(std::round(state.lastX)), int(std::round(state.lastY)), pressure);

        state.accumulatedDistance -= spacingPx;
    }
}

/*void BrushTool::stamp(Frame* frame, int cx, int cy, float pressure)
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
*/
void BrushTool::stamp(Frame* frame, StrokeCommand* cmd, int cx, int cy, float pressure)
{
    PixelBuffer& pb = frame->pixels();

    float R = float(m_settings.radius);

    if (m_settings.sizePressure) R *= pressure;

    if (R <= 0.0f) return;

    int ir = int(std::ceil(R));

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

            if (alpha <= 0.0f) continue;

            if (alpha > 1.0f) alpha = 1.0f;

            PixelRGBA8 before = pb.getPixel(px, py);
            PixelRGBA8 after = before;

            if (m_mode == BrushBlendMode::Normal)
            {
                PixelRGBA8 src = m_color;
                src.a = uint8_t(src.a * alpha);

                float sa = src.a / 255.0f;
                float da = before.a / 255.0f;
                float oa = sa + da * (1.0f - sa);

                if (oa > 0.0f)
                {
                    after.r = uint8_t((src.r * sa + before.r * da * (1.0f - sa)) / oa);
                    after.g = uint8_t((src.g * sa + before.g * da * (1.0f - sa)) / oa);
                    after.b = uint8_t((src.b * sa + before.b * da * (1.0f - sa)) / oa);
                    after.a = uint8_t(oa * 255.0f);
                }
            }
            else if (m_mode == BrushBlendMode::Erase)
            {
                after.a = uint8_t(before.a * (1.0f - alpha));
            }

            if (before.r == after.r &&
                before.g == after.g &&
                before.b == after.b &&
                before.a == after.a)
                continue;

            cmd->recordPixel(px, py, before, after);
            pb.setPixel(px, py, after);
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