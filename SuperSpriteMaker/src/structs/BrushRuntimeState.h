#pragma once
struct BrushRuntimeState {
	float accumulatedDistance = 0.0f;
	float lastX = 0.0f;
	float lastY = 0.0f;
	bool first = true;
};