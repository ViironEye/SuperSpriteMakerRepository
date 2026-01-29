#include "Tool.h"

class BrushTool : public Tool {
public:
	BrushTool(const PixelRGBA8& color, int radius) : m_color(color), m_radius(radius) { }

	void apply(Frame* frame, int cx, int cy) override
	{
		if (!frame) return;

		PixelBuffer& buf = frame->pixels();

		for (int y = -m_radius; y <= m_radius; ++y) 
		{
			for (int x = -m_radius; x <= m_radius; ++x) 
			{
				int px = cx + x;
				int py = cy + y;

				if (px < 0 || py < 0 || px >= buf.width() || py >= buf.height())
				{
					continue;
				}

				if (x * x + y * y > m_radius * m_radius)
				{
					continue;
				}

				PixelRGBA8 dst = buf.getPixel(px, py);
				PixelRGBA8 out;

				uint8_t invA = 255 - m_color.a;
				out.a = m_color.a + (dst.a * invA) / 255;
				out.r = (m_color.r * m_color.a + dst.r * invA) / 255;
				out.g = (m_color.g * m_color.a + dst.g * invA) / 255;
				out.b = (m_color.b * m_color.a + dst.b * invA) / 255;

				buf.setPixel(px, py, out);
			}
		}
	}

private:
	PixelRGBA8 m_color;
	int m_radius = 1;
};