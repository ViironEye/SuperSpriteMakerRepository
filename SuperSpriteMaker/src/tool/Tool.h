#pragma once
#include "../model/Frame.h"
#include "../structs/BrushRuntimeState.h"
#include "StrokeCommand.h"

class Tool {
public:
	virtual ~Tool() = default;

	virtual void apply(Frame* frame, StrokeCommand* cmd, BrushRuntimeState& state, int x, int y, float pressure) = 0;
};