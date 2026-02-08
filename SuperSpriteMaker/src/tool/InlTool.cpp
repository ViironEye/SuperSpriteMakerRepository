#include "InkTool.h"
#include "../model/Frame.h"
#include <cmath>

PixelRGBA8 InkTool::blendSrcOver(const PixelRGBA8& dst, const PixelRGBA8& src)
{
    // Нормализованные альфы
    float sa = src.a / 255.0f;
    float da = dst.a / 255.0f;

    // outA = sa + da*(1-sa)
    float oa = sa + da * (1.0f - sa);
    if (oa <= 0.0f)
        return PixelRGBA8(0, 0, 0, 0);

    // Premultiplied подход (без хранения premul в буфере)
    float sr = src.r / 255.0f;
    float sg = src.g / 255.0f;
    float sb = src.b / 255.0f;

    float dr = dst.r / 255.0f;
    float dg = dst.g / 255.0f;
    float db = dst.b / 255.0f;

    float or_ = (sr * sa + dr * da * (1.0f - sa)) / oa;
    float og_ = (sg * sa + dg * da * (1.0f - sa)) / oa;
    float ob_ = (sb * sa + db * da * (1.0f - sa)) / oa;

    return PixelRGBA8(
        clampU8(int(std::round(or_ * 255.0f))),
        clampU8(int(std::round(og_ * 255.0f))),
        clampU8(int(std::round(ob_ * 255.0f))),
        clampU8(int(std::round(oa * 255.0f)))
    );
}

void InkTool::apply(Frame* frame,
    StrokeCommand* cmd,
    int x, int y,
    float pressure)
{
    if (!frame || !cmd)
        return;

    PixelBuffer& pb = frame->pixels();
    if (!pb.inBounds(x, y))
        return;

    // Нормализуем pressure
    if (pressure < 0.0f) pressure = 0.0f;
    if (pressure > 1.0f) pressure = 1.0f;

    PixelRGBA8 before = pb.getPixel(x, y);

    // Ink: 1px, но альфа зависит от pressure
    PixelRGBA8 src = m_color;
    int newA = int(std::round(src.a * pressure));
    src.a = clampU8(newA);

    // Если совсем прозрачный — ничего не делаем
    if (src.a == 0)
        return;

    PixelRGBA8 after = blendSrcOver(before, src);

    if (before.r == after.r &&
        before.g == after.g &&
        before.b == after.b &&
        before.a == after.a)
        return;

    cmd->recordPixel(x, y, before, after);
    pb.setPixel(x, y, after);
}
