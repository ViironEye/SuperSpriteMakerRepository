#pragma once
#include "../model/Frame.h"
#include "StrokeCommand.h"

class Tool {
public:
	virtual ~Tool() = default;

	virtual void apply(Frame* frame, StrokeCommand* cmd, int x, int y) = 0;
};