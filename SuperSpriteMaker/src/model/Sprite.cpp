#include "Sprite.h"

Sprite::Sprite(int width, int height, PixelFormat fmt) : m_width(width), m_height(height), m_format(fmt) { }

Frame* Sprite::createFrame(int durationMs)
{
	m_frames.push_back(std::make_unique<Frame>(m_width, m_height, m_format, durationMs));
	return m_frames.back().get();
}

Frame* Sprite::getFrame(int index)
{
	if (index < 0 || index >= (int)m_frames.size())
	{
		return nullptr;
	}
	return m_frames[index].get();
}

void Sprite::pruneUnusedFrames()
{
	std::vector<bool> used(m_frames.size(), false);

	for (const auto& layer : m_layers) 
	{
		for (int i = 0; i < (int)used.size(); ++i) 
		{
			Cel* cel = layer->getCel(i);
			if (!cel) continue;

			Frame* f = cel->frame();

			for (size_t fi = 0; fi < m_frames.size(); ++fi) 
			{
				if (m_frames[fi].get() == f) 
				{
					used[fi] = true;
				}
			}
		}
	}

	for (int i = (int)m_frames.size() - 1; i >= 0; --i) 
	{
		if (!used[i]) 
		{
			m_frames.erase(m_frames.begin() + i);
		}
	}
}

Layer* Sprite::createLayer(const std::string& name)
{
	m_layers.push_back(std::make_unique<Layer>(name));
	return m_layers.back().get();
}

Layer* Sprite::getLayer(int index)
{
	if (index < 0 || index >= (int)m_layers.size())
	{
		return nullptr;
	}
	return m_layers[index].get();
}

bool Sprite::removeLayer(int index)
{
	if (index < 0 || index >= (int)m_layers.size())
	{
		return false;
	}

	m_layers.erase(m_layers.begin() + index);
	return true;
}

bool Sprite::removeFrame(int index)
{
	if (index < 0 || index >= (int)m_frames.size())
	{
		return false;
	}

	for (auto& layer : m_layers) {

		for (int i = 0; i < layerCount(); ++i) 
		{
			Cel* cel = layer->getCel(i);

			if (cel && cel->frame() == m_frames[index].get()) 
			{
				layer->addCel(i, nullptr);
			}
		}
	}

	m_frames.erase(m_frames.begin() + index);
	return true;
}