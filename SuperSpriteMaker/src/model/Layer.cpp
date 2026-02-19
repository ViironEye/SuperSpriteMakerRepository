#include "Layer.h"
#include "Cel.h"

void Layer::ensureFrameCount(int frames)
{
    if (frames < 0) frames = 0;
    if ((int)m_cels.size() < frames)
        m_cels.resize((size_t)frames);
}

Cel* Layer::getCel(int frameIndex)
{
    if (frameIndex < 0 || frameIndex >= (int)m_cels.size()) return nullptr;
    return m_cels[(size_t)frameIndex].get();
}

const Cel* Layer::getCel(int frameIndex) const
{
    if (frameIndex < 0 || frameIndex >= (int)m_cels.size()) return nullptr;
    return m_cels[(size_t)frameIndex].get();
}

void Layer::setCel(int frameIndex, std::unique_ptr<Cel> cel)
{
    if (frameIndex < 0) return;
    ensureFrameCount(frameIndex + 1);
    m_cels[(size_t)frameIndex] = std::move(cel);
}

void Layer::deleteCel(int frameIndex)
{
    if (frameIndex < 0 || frameIndex >= (int)m_cels.size()) return;
    m_cels[(size_t)frameIndex].reset();
}

void Layer::eraseFrame(int frameIndex)
{
    if (frameIndex < 0 || frameIndex >= (int)m_cels.size())
        return;
    m_cels.erase(m_cels.begin() + frameIndex);
}