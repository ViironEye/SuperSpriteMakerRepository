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
	static uint32_t blendPixel(uint32_t dst, uint32_t src);
};