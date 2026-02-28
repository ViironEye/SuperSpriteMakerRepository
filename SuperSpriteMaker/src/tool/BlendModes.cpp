#include "BlendModes.h"

float Blend::blendChannel(BlendMode mode, float d, float s)
{
    switch (mode) {
    case BlendMode::Normal:   return s;
    case BlendMode::Multiply: return d * s;
    case BlendMode::Screen:   return 1.0f - (1.0f - d) * (1.0f - s);
    case BlendMode::Add:      return d + s;
    case BlendMode::Subtract: return d - s;
    case BlendMode::Darken:   return (d < s) ? d : s;
    case BlendMode::Lighten:  return (d > s) ? d : s;
    default:                  return s;
    }
}

PixelRGBA8 Blend::composite(const PixelRGBA8& dst, const PixelRGBA8& src, uint8_t srcOpacity, BlendMode mode)
{
    float da = u8To01(dst.a);
    float sa = u8To01(src.a);

    float oa = u8To01(srcOpacity);
    sa *= oa;

    if (sa <= 0.0f) return dst;

    float dr = u8To01(dst.r);
    float dg = u8To01(dst.g);
    float db = u8To01(dst.b);

    float sr = u8To01(src.r);
    float sg = u8To01(src.g);
    float sb = u8To01(src.b);

    float br = blendChannel(mode, dr, sr);
    float bg = blendChannel(mode, dg, sg);
    float bb = blendChannel(mode, db, sb);

    float outA = sa + da * (1.0f - sa);

    float outR_p = br * sa + dr * da * (1.0f - sa);
    float outG_p = bg * sa + dg * da * (1.0f - sa);
    float outB_p = bb * sa + db * da * (1.0f - sa);

    float outR = (outA > 0.0f) ? (outR_p / outA) : 0.0f;
    float outG = (outA > 0.0f) ? (outG_p / outA) : 0.0f;
    float outB = (outA > 0.0f) ? (outB_p / outA) : 0.0f;

    PixelRGBA8 out;
    out.r = f01ToU8(outR);
    out.g = f01ToU8(outG);
    out.b = f01ToU8(outB);
    out.a = f01ToU8(outA);
    return out;
}

PixelRGBA8 Blend::apply(BlendMode mode, const PixelRGBA8& dst, const PixelRGBA8& src, uint8_t srcOpacity)
{
    return composite(dst, src, srcOpacity, mode);
}
