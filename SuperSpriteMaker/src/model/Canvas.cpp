#include "Canvas.h"

Canvas::Canvas(Sprite* sprite) : m_sprite(sprite) { }

PixelBuffer Canvas::renderFrame(int frameIndex) const
{
	PixelBuffer result(
		m_sprite->width(),
		m_sprite->height(),
		m_sprite->format()
	);

	result.clear(0x00000000);

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

				if (dx < 0 || dy < 0 || dx >= result.width() || dy >= result.height()) continue;

				uint32_t srcPx = src.getPixel(x, y);
				uint32_t& dstPx = result.pixelRef(dx, dy);

				dstPx = blendPixel(dstPx, srcPx);
			}
		}
	}

	return result;
}


uint32_t Canvas::blendPixel(uint32_t dst, uint32_t src)
{
	uint8_t sa = (src >> 24) & 0xFF;
	if (sa == 255)
		return src;
	if (sa == 0)
		return dst;

	uint8_t sr = (src >> 16) & 0xFF;
	uint8_t sg = (src >> 8) & 0xFF;
	uint8_t sb = src & 0xFF;

	uint8_t da = (dst >> 24) & 0xFF;
	uint8_t dr = (dst >> 16) & 0xFF;
	uint8_t dg = (dst >> 8) & 0xFF;
	uint8_t db = dst & 0xFF;

	uint8_t outA = sa + da * (255 - sa) / 255;
	uint8_t outR = (sr * sa + dr * (255 - sa)) / 255;
	uint8_t outG = (sg * sa + dg * (255 - sa)) / 255;
	uint8_t outB = (sb * sa + db * (255 - sa)) / 255;

	return (outA << 24) | (outR << 16) | (outG << 8) | outB;
}