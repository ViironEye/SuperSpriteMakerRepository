#include "SpriteEditor.h"

SelectionOp SpriteEditor::selectionOpFromMods(const Modifiers& mods) const
{
    if (mods.ctrl)  return SelectionOp::Subtract;
    if (mods.shift) return SelectionOp::Add;
    return SelectionOp::Replace;
}
static inline uint8_t mul255(uint8_t a, uint8_t b)
{
    return (uint8_t)((int(a) * int(b) + 127) / 255);
}

static void blendOver(PixelBuffer& dst, const PixelBuffer& src, int dx, int dy, uint8_t layerOpacity)
{
    for (int y = 0; y < src.height(); ++y)
    {
        int ty = dy + y;
        if (ty < 0 || ty >= dst.height()) continue;

        for (int x = 0; x < src.width(); ++x)
        {
            int tx = dx + x;
            if (tx < 0 || tx >= dst.width()) continue;

            PixelRGBA8 s = src.getPixel(x, y);
            if (s.a == 0) continue;

            uint8_t Sa = mul255(s.a, layerOpacity);
            if (Sa == 0) continue;

            PixelRGBA8 d = dst.getPixel(tx, ty);

            float sa = Sa / 255.0f;
            float da = d.a / 255.0f;
            float outA = sa + da * (1.0f - sa);

            if (outA <= 0.0f) 
            {
                dst.setPixel(tx, ty, PixelRGBA8(0, 0, 0, 0));
                continue;
            }

            auto blendChan = [&](uint8_t sc, uint8_t dc) -> uint8_t
                {
                    float sr = sc / 255.0f;
                    float dr = dc / 255.0f;
                    float out = (sr * sa + dr * da * (1.0f - sa)) / outA;
                    int v = (int)std::lround(out * 255.0f);
                    if (v < 0) v = 0; if (v > 255) v = 255;
                    return (uint8_t)v;
                };

            PixelRGBA8 out;
            out.r = blendChan(s.r, d.r);
            out.g = blendChan(s.g, d.g);
            out.b = blendChan(s.b, d.b);
            out.a = (uint8_t)std::lround(outA * 255.0f);

            dst.setPixel(tx, ty, out);
        }
    }
}

void SpriteEditor::pointerDown(int x, int y, float pressure, const Modifiers& mods)
{
    Frame* frame = activeCelFrame();
    if (!frame) return;

    if (m_mode == EditorMode::Draw)
    {
        if (!m_tool) return;

        m_strokeCmd = std::make_unique<StrokeCommand>(frame);
        m_stroke = std::make_unique<Stroke>(m_tool, frame, m_strokeCmd.get());

        m_stroke->begin(x, y, pressure);
        return;
    }

    if (m_mode == EditorMode::SelectRect)
    {
        m_rectSelect.begin(x, y, selectionOpFromMods(mods));
        m_rectSelect.update(x, y);
        return;
    }

    if (m_mode == EditorMode::MoveSelect)
    {
        if (!m_moveSession.active())
        {
            MoveMode mm = m_moveMode;
            if (mods.alt) mm = MoveMode::Copy;

            if (!m_moveSession.begin(frame, m_selection, mm))
                return;
        }

        m_draggingMove = true;
        m_moveStartX = x;
        m_moveStartY = y;
        m_moveSession.setOffset(0, 0);
        return;
    }

    if (m_mode == EditorMode::Draw)
    {
        if (!m_tool) return;

        Frame* frame = activeCelFrame();
        if (!frame) return;

        if (auto* fill = dynamic_cast<FillTool*>(m_tool))
        {
            auto cmd = std::make_unique<StrokeCommand>(frame);
            fill->apply(frame, cmd.get(), x, y, pressure);
            m_undo.push(std::move(cmd));
            return;
        }

        m_strokeCmd = std::make_unique<StrokeCommand>(frame);
        m_stroke = std::make_unique<Stroke>(m_tool, frame, m_strokeCmd.get());
        m_stroke->begin(x, y, pressure);
        return;
    }

    if (m_mode == EditorMode::Draw)
    {
        if (!m_tool) return;

        Frame* frame = activeCelFrame();
        if (!frame) return;

        if (dynamic_cast<EyedropperTool*>(m_tool))
        {
            m_tool->apply(frame, nullptr, x, y, pressure);
            return;
        }

        if (auto* fill = dynamic_cast<FillTool*>(m_tool))
        {
            auto cmd = std::make_unique<StrokeCommand>(frame);
            fill->apply(frame, cmd.get(), x, y, pressure);
            m_undo.push(std::move(cmd));
            return;
        }
    }
}

void SpriteEditor::pointerMove(int x, int y, float pressure, const Modifiers& mods)
{
    (void)mods;

    if (m_mode == EditorMode::Draw)
    {
        if (m_stroke)
            m_stroke->update(x, y, pressure);
        return;
    }

    if (m_mode == EditorMode::SelectRect)
    {
        if (m_rectSelect.active())
            m_rectSelect.update(x, y);
        return;
    }

    if (m_mode == EditorMode::MoveSelect)
    {
        if (m_moveSession.active() && m_draggingMove)
        {
            int dx = x - m_moveStartX;
            int dy = y - m_moveStartY;
            m_moveSession.setOffset(dx, dy);
        }
        return;
    }
}

void SpriteEditor::pointerUp(int x, int y, float pressure, const Modifiers& mods)
{
    (void)x; 
    (void)y; 
    (void)pressure; 
    (void)mods;

    if (m_mode == EditorMode::Draw)
    {
        if (m_stroke) {
            m_stroke->end(x, y, pressure);
            m_stroke.reset();
        }

        if (m_strokeCmd) {
            m_undo.push(std::move(m_strokeCmd));
        }
        return;
    }

    if (m_mode == EditorMode::SelectRect)
    {
        m_rectSelect.end(m_selection);
        return;
    }

    if (m_mode == EditorMode::MoveSelect)
    {
        m_draggingMove = false;
        return;
    }
}

void SpriteEditor::keyEsc()
{
    if (m_moveSession.active())
        m_moveSession.cancel();
}

void SpriteEditor::keyEnter()
{
    if (!m_moveSession.active()) return;

    auto cmd = m_moveSession.commit();
    if (cmd) m_undo.push(std::move(cmd));
}

const PixelBuffer& SpriteEditor::compositeFrame()
{
    Frame* baseFrame = activeCelFrame();
    if (!baseFrame) return m_composite;

    if (m_composite.width() != m_sprite->width() || m_composite.height() != m_sprite->height())
        m_composite = PixelBuffer(m_sprite->width(), m_sprite->height(), PixelFormat::RGBA8);

    m_composite.clear(PixelRGBA8(0, 0, 0, 0));

    int fi = m_activeFrame;

    for (int li = 0; li < m_sprite->layerCount(); ++li)
    {
        Layer* L = m_sprite->getLayer(li);
        if (!L || !L->visible()) continue;

        Cel* cel = L->getCel(fi);
        if (!cel || !cel->frame()) continue;

        const PixelBuffer& src = cel->frame()->pixels();
        uint8_t layerOpacity = L->opacity();

        blendOver(m_composite, src, cel->x(), cel->y(), layerOpacity);
    }

    return m_composite;
}

const PixelBuffer& SpriteEditor::compositePixels()
{
    if (!m_sprite) return m_composite;

    int W = m_sprite->width();
    int H = m_sprite->height();

    if (m_composite.width() != W || m_composite.height() != H)
        m_composite = PixelBuffer(W, H, PixelFormat::RGBA8);

    m_composite.clear(PixelRGBA8(0, 0, 0, 0));

    for (int li = 0; li < m_sprite->layerCount(); ++li)
    {
        Layer* L = m_sprite->getLayer(li);
        if (!L || !L->visible()) continue;

        Cel* c = L->getCel(m_activeFrame);
        if (!c) continue;

        blendOver(m_composite, c->pixels(), c->x(), c->y(), L->opacity());
    }

    return m_composite;
}

void SpriteEditor::clampActiveIndices()
{
    if (!m_sprite) return;

    if (m_activeFrame < 0) m_activeFrame = 0;
    if (m_activeFrame >= m_sprite->frameCount()) m_activeFrame = std::max(0, m_sprite->frameCount() - 1);

    if (m_activeLayer < 0) m_activeLayer = 0;
    if (m_activeLayer >= m_sprite->layerCount()) m_activeLayer = std::max(0, m_sprite->layerCount() - 1);
}