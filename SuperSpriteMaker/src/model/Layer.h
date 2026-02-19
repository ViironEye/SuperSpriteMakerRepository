#pragma once
#include "Cel.h"
#include "../enums/BlendMode.h"
#include <memory>

class Cel;

class Layer
{
public:
    explicit Layer(const std::string& name) : m_name(name) {}

    void ensureFrameCount(int frames);
    void addEmptyCel(int frameIndex);
    Cel* getCel(int frameIndex);
    const Cel* getCel(int frameIndex) const;

    void setCel(int frameIndex, std::unique_ptr<Cel> cel);
    void deleteCel(int frameIndex);

    const std::string& name() const { return m_name; }
    void setName(const std::string& n) { m_name = n; }

    bool visible() const { return m_visible; }
    void setVisible(bool v) { m_visible = v; }

    uint8_t opacity() const { return m_opacity; }
    void setOpacity(uint8_t o) { m_opacity = o; }

    BlendMode blendMode() const { return m_blendMode; }
    void setBlendMode(BlendMode m) { m_blendMode = m; }
    
    void eraseFrame(int frameIndex);

private:
    std::string m_name;
    bool m_visible = true;
    uint8_t m_opacity = 255;
    BlendMode m_blendMode = BlendMode::Normal;

    std::vector<std::unique_ptr<Cel>> m_cels;
};