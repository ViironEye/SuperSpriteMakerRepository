#pragma once
#include "Layer.h"

struct TimelineFrame
{
    int durationMs = 100;
};

class Sprite {
public:
    Sprite(int width, int height, PixelFormat fmt);

    int width() const { return m_width; }
    int height() const { return m_height; }
    PixelFormat format() const { return m_format; }

    int frameCount() const { return (int)m_frames.size(); }
    int layerCount() const { return (int)m_layers.size(); }

    TimelineFrame& frameInfo(int i) { return m_frames[i]; }
    const TimelineFrame& frameInfo(int i) const { return m_frames[i]; }

    int createFrame(int durationMs);
    bool removeFrame(int index);

    Layer* createLayer(const std::string& name);
    bool removeLayer(int index);

    Layer* getLayer(int index);
    const Layer* getLayer(int index) const;

private:
    int m_width, m_height;
    PixelFormat m_format;

    std::vector<TimelineFrame> m_frames;
    std::vector<std::unique_ptr<Layer>> m_layers;
};