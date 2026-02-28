#pragma once
#include <vector>
#include <memory>
#include "../model/Frame.h"

class Command 
{
public:
	virtual ~Command() = default;
	virtual void undo() = 0;
	virtual void redo() = 0;
};

struct PixelDiff 
{
	int x;
	int y;
	PixelRGBA8 before;
	PixelRGBA8 after;
};