#pragma once
#include "Command.h"

class StrokeCommand : public Command {
public:
	StrokeCommand(Frame* frame) : m_frame(frame) { }

	void recordPixel(int x, int y, const PixelRGBA8& before, const PixelRGBA8& after)
	{
		if (before.r == after.r && before.g == after.g &&
			before.b == after.b && before.a == after.a)
			return;

		m_diffs.push_back({ x, y, before, after });
	}

	void undo() override
	{
		PixelBuffer& buf = m_frame->pixels();
		for (const PixelDiff& d : m_diffs) 
		{
			buf.setPixel(d.x, d.y, d.before);
		}
	}

	void redo() override
	{
		PixelBuffer& buf = m_frame->pixels();
		for (const PixelDiff& d : m_diffs) 
		{
			buf.setPixel(d.x, d.y, d.after);
		}
	}

private:
	Frame* m_frame = nullptr;
	std::vector<PixelDiff> m_diffs;
};