#pragma once
#include <cstdint>

struct BrushSettings {
	float radius = 1.0f;
	float spacing = 0.25f;
	float opacity = 1.0f;
	float hardness = 1.0f;

	bool sizePressure = true;
	bool opacityPressure = true;

	BrushSettings() = default;

	BrushSettings(float r, float s, float o, float h) : radius(r), spacing(s), opacity(o), hardness(h) { }

	float falloff(float distance) const 
	{
		if (distance >= radius) return 0.0f;

		float t = 1.0f - (distance / radius);

		return hardness >= 1.0f ? 1.0f : t * hardness + t * t * (1.0f - hardness);
	}
};