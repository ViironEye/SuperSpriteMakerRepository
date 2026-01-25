#include "Layer.h"

Layer::Layer(const std::string& name) : m_name(name) { }

void Layer::addCel(int frameIndex, Cel* cel)
{
	if (frameIndex < 0) return;

	if (frameIndex >= (int)m_cels.size())
	{
		m_cels.resize(frameIndex + 1, nullptr);
	}

	m_cels[frameIndex] = cel;
}

Cel* Layer::getCel(int frameIndex)
{
	if (frameIndex < 0 || frameIndex >= (int)m_cels.size())
	{
		return nullptr;
	}
	return m_cels[frameIndex];
}

const Cel* Layer::getCel(int frameIndex) const
{
	if (frameIndex < 0 || frameIndex >= (int)m_cels.size())
	{
		return nullptr;
	}
	return m_cels[frameIndex];
}

void Layer::setCel(int frameIndex, Cel* cel)
{
	if (frameIndex < 0)
	{
		return;
	}

	if (frameIndex >= (int)m_cels.size())
	{
		m_cels.resize(frameIndex + 1);
	}

	m_cels[frameIndex] = std::move(cel);
}

void Layer::deleteCel(int frameIndex)
{
	if (frameIndex < 0 || frameIndex >= (int)m_cels.size())
	{
		return;
	}

	delete m_cels[frameIndex];
	m_cels[frameIndex] = nullptr;
}