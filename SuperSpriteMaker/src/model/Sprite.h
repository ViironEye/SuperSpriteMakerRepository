#pragma once
#include "Frame.h"
#include "Layer.h"
#include <vector>
#include <memory>

class Sprite {
public:
	Sprite(int width, int height, PixelFormat fmt);

	Frame* createFrame(int durationMs = 100);
	Frame* getFrame(int index);
	bool removeFrame(int index);
	int frameCount() const { return (int)m_frames.size(); }

	void pruneUnusedFrames();

	Layer* createLayer(const std::string& name);
	Layer* getLayer(int index);
	bool removeLayer(int index);
	int layerCount() const { return (int)m_layers.size(); }

	int width() const { return m_width; }
	int height() const { return m_height; }
	PixelFormat format() const { return m_format; }

private:
	int m_width;
	int m_height;
	PixelFormat m_format;

	std::vector<std::unique_ptr<Frame>> m_frames;
	std::vector<std::unique_ptr<Layer>> m_layers;
};