#include "SpriteEditor.h"

SelectionOp SpriteEditor::selectionOpFromMods(const Modifiers& mods) const
{
    if (mods.ctrl)  return SelectionOp::Subtract;
    if (mods.shift) return SelectionOp::Add;
    return SelectionOp::Replace;
}

void SpriteEditor::pointerDown(int x, int y, float pressure, const Modifiers& mods)
{
    Frame* frame = activeFrame();
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
    (void)x; (void)y; (void)pressure; (void)mods;

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
    if (!m_moveSession.active())
        return;

    auto cmd = m_moveSession.commit();
    if (cmd)
        m_undo.push(std::move(cmd));
}
