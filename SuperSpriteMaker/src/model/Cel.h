#pragma once
#include "Frame.h"
class Cel {
public:
	Cel(Frame* frame, int x = 0, int y = 0) : m_frame(frame), m_x(x), m_y(y) { }

	Frame* frame() { return m_frame; }
	const Frame* frame() const { return m_frame; }

	int x() const { return m_x; }
	int y() const { return m_y; }

	void setX(int v) { m_x = v; }
	void setY(int v) { m_y = v; }

private:
	Frame* m_frame;
	int m_x;
	int m_y;
};