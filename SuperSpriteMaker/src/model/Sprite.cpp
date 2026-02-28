#include "Sprite.h"
#include "Sprite.h"
#include "Layer.h"
#include "Cel.h"

Sprite::Sprite(int width, int height, PixelFormat fmt)
    : m_width(width), m_height(height), m_format(fmt) {
}

int Sprite::createFrame(int durationMs)
{
    m_frames.push_back(TimelineFrame{ durationMs });

    int newIndex = (int)m_frames.size() - 1;
    for (auto& L : m_layers) L->ensureFrameCount((int)m_frames.size());

    return newIndex;
}

bool Sprite::removeFrame(int index)
{
    if (index < 0 || index >= (int)m_frames.size()) return false;

    for (auto& L : m_layers) L->eraseFrame(index);

    m_frames.erase(m_frames.begin() + index);

    return true;
}

Layer* Sprite::createLayer(const std::string& name)
{
    m_layers.push_back(std::make_unique<Layer>(name));
    Layer* L = m_layers.back().get();

    L->ensureFrameCount((int)m_frames.size());

    for (int fi = 0; fi < (int)m_frames.size(); ++fi)
    {
        L->setCel(fi, std::make_unique<Cel>(m_width, m_height, m_format, m_frames[fi].durationMs));
    }

    return L;
}

bool Sprite::removeLayer(int index)
{
    if (index < 0 || index >= (int)m_layers.size()) return false;
    m_layers.erase(m_layers.begin() + index);
    return true;
}

Layer* Sprite::getLayer(int index)
{
    if (index < 0 || index >= (int)m_layers.size()) return nullptr;
    return m_layers[index].get();
}

const Layer* Sprite::getLayer(int index) const
{
    if (index < 0 || index >= (int)m_layers.size()) return nullptr;
    return m_layers[index].get();
}