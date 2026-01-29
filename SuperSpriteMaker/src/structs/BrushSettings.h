#pragma once
#include <cstdint>

struct BrushSettings {
	int radius = 1;
	float opacity = 1.0f;
	float hardness = 1.0f;

	BrushSettings() = default;

	BrushSettings(int r, float o, float h) : radius(r), opacity(o), hardness(h) { }

	float falloff(float distance) const 
	{
		if (distance >= radius) return 0.0f;

		float t = 1.0f - (distance / radius);

		return hardness >= 1.0f ? 1.0f : t * hardness + t * t * (1.0f - hardness);
	}
};