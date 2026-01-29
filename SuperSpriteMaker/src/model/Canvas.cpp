#include "Canvas.h"

Canvas::Canvas(Sprite* sprite) : m_sprite(sprite) { }

PixelBuffer Canvas::renderFrame(int frameIndex) const
{
	PixelBuffer result(
		m_sprite->width(),
		m_sprite->height(),
		m_sprite->format()
	);

	result.clear(PixelRGBA8(0, 0, 0, 0));

	for (int li = 0; li < m_sprite->layerCount(); ++li) 
	{
		const Layer* layer = m_sprite->getLayer(li);
		if (!layer || !layer->visible()) continue;

		const Cel* cel = layer->getCel(frameIndex);
		if (!cel) continue;

		const Frame* frame = cel->frame();
		const PixelBuffer& src = frame->pixels();

		int ox = cel->x();
		int oy = cel->y();

		for (int y = 0; y < src.height(); ++y) 
		{
			for (int x = 0; x < src.width(); ++x) 
			{
				int dx = x + ox;
				int dy = y + oy;

				if (dx < 0 || dy < 0 || dx >= result.width() || dy >= result.height())
				{
					continue;
				}

				PixelRGBA8 srcPx = src.getPixel(x, y);
				PixelRGBA8 dstPx = result.getPixel(dx, dy);

				PixelRGBA8 out = blendPixel(dstPx, srcPx);
				result.setPixel(dx, dy, out);
			}
		}
	}

	return result;
}

PixelRGBA8 Canvas::blendPixel(const PixelRGBA8& dst, const PixelRGBA8& src) 
{
	if (src.a == 255)
		return src;
	if (src.a == 0)
		return dst;

	uint8_t invA = 255 - src.a;

	PixelRGBA8 out;
	out.a = src.a + (dst.a * invA) / 255;
	out.r = (src.r * src.a + dst.r * invA) / 255;
	out.g = (src.g * src.a + dst.g * invA) / 255;
	out.b = (src.b * src.a + dst.b * invA) / 255;

	return out;
}