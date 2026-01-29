#include <cmath>
#include "../structs/BrushSettings.h"
#include "Tool.h"

class BrushTool : public Tool {
public:
	BrushTool(const PixelRGBA8& color, const BrushSettings& settings) : m_color(color), m_settings(settings) { }

	void apply(Frame* frame, StrokeCommand* cmd, int cx, int cy) override
	{
		PixelBuffer& buf = frame->pixels();

		const int r = m_settings.radius;

		for (int y = -r; y <= r; ++y) 
		{
			for (int x = -r; x <= r; ++x) 
			{
				const int px = cx + x;
				const int py = cy + y;

				if (!buf.inBounds(px, py)) continue;

				const float dist = std::sqrt(float(x * x + y * y));
				if (dist >= r) continue;

				const float shape = m_settings.falloff(dist);
				if (shape <= 0.0f) continue;

				const float alphaF = (m_color.a / 255.0f) * m_settings.opacity * shape;

				if (alphaF <= 0.0f) continue;

				const uint8_t alpha = uint8_t(alphaF * 255.0f);

				PixelRGBA8 before = buf.getPixel(px, py);
				PixelRGBA8 after = blend(before, m_color, alpha);

				if (before.r == after.r && before.g == after.g &&
					before.b == after.b && before.a == after.a)
					continue;

				cmd->recordPixel(px, py, before, after);
				buf.setPixel(px, py, after);
			}
		}
	}

private:
	PixelRGBA8 m_color;
	BrushSettings m_settings;

	static PixelRGBA8 blend(const PixelRGBA8& dst, const PixelRGBA8& src, uint8_t alpha)
	{
		const float a = alpha / 255.0f;
		const float ia = 1.0f - a;

		return PixelRGBA8(
			uint8_t(dst.r * ia + src.r * a),
			uint8_t(dst.g * ia + src.g * a),
			uint8_t(dst.b * ia + src.b * a),
			255
		);
	}
};