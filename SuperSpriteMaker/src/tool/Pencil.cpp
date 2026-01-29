#include "Tool.h"

class PencilTool : public Tool {
public:
	explicit PencilTool(const PixelRGBA8& color) : m_color(color) { }

	void setColor(const PixelRGBA8& c) { m_color = c; }

	void apply(Frame* frame, int x, int y) override
	{
		if (!frame) return;

		PixelBuffer& buf = frame->pixels();

		if (x < 0 || y < 0 || x >= buf.width() || y >= buf.height())
		{
			return;
		}

		buf.setPixel(x, y, m_color);
	}

private:
	PixelRGBA8 m_color;
};