#pragma once
#include "../model/Frame.h"

class Tool {
public:
	virtual ~Tool() = default;

	virtual void apply(Frame* frame, int x, int y) = 0;
};