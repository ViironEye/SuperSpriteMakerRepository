#pragma once
#include <vector>
#include "Sprite.h"
#include "PixelBuffer.h"

class Canvas {
public:
	explicit Canvas(Sprite* sprite);
	PixelBuffer renderFrame(int frameIndex) const;

private:
	Sprite* m_sprite = nullptr;
	static PixelRGBA8 blendPixel(const PixelRGBA8& dst, const PixelRGBA8& src);
};