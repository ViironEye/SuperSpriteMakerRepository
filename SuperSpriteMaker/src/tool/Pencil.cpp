#include "Tool.h"

class PencilTool : public Tool {
public:
	explicit PencilTool(PixelRGBA8 color) : m_color(color) { m_color.a = 255; }

	void apply(Frame* frame, StrokeCommand* cmd, int x, int y) override
	{
		PixelBuffer& buf = frame->pixels();
		if (!buf.inBounds(x, y)) return;

		PixelRGBA8 before = buf.getPixel(x, y);
		PixelRGBA8 after = m_color;

		if (before.r == after.r && before.g == after.g &&
			before.b == after.b && before.a == after.a)
			return;

		cmd->recordPixel(x, y, before, after);
		buf.setPixel(x, y, after);
	}

private:
	PixelRGBA8 m_color;
};