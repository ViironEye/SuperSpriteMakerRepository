#pragma once
#pragma once
#include <memory>
#include <cstdint>

#include "../model/Sprite.h"
#include "../model/Frame.h"
#include "../tool/UndoStack.h"
#include "../tool/Stroke.h"
#include "../tool/SelectionMask.h"
#include "../tool/SelectionMoveSession.h"
#include "../tool/RectSelectTool.h"
#include "../tool/Tool.h"

enum class EditorMode {
    Draw,
    SelectRect,
    MoveSelect
};

struct Modifiers {
    bool shift = false;
    bool ctrl = false;
    bool alt = false;
};

class SpriteEditor {
public:
    explicit SpriteEditor(Sprite* sprite)
        : m_sprite(sprite)
    {
        if (m_sprite)
            m_selection.resize(m_sprite->width(), m_sprite->height());
    }

    void setActiveFrameIndex(int i) { m_activeFrame = i; }
    int  activeFrameIndex() const { return m_activeFrame; }
    Frame* activeFrame() const { return m_sprite ? m_sprite->getFrame(m_activeFrame) : nullptr; }

    void setTool(Tool* tool) { m_tool = tool; }
    Tool* tool() const { return m_tool; }

    void setMode(EditorMode m) { m_mode = m; }
    EditorMode mode() const { return m_mode; }

    SelectionMask& selection() { return m_selection; }
    const SelectionMask& selection() const { return m_selection; }

    UndoStack& undo() { return m_undo; }

    void pointerDown(int x, int y, float pressure, const Modifiers& mods);
    void pointerMove(int x, int y, float pressure, const Modifiers& mods);
    void pointerUp(int x, int y, float pressure, const Modifiers& mods);

    void keyEsc();
    void keyEnter();

    int selectionPhase() const { return m_phase; }
    void tick() { ++m_phase; }

    SelectionMoveSession& moveSession() { return m_moveSession; }
    const SelectionMoveSession& moveSession() const { return m_moveSession; }

private:
    SelectionOp selectionOpFromMods(const Modifiers& mods) const;

private:
    Sprite* m_sprite = nullptr;
    int m_activeFrame = 0;

    EditorMode m_mode = EditorMode::Draw;
    Tool* m_tool = nullptr;

    UndoStack m_undo;

    std::unique_ptr<StrokeCommand> m_strokeCmd;
    std::unique_ptr<Stroke> m_stroke;

    SelectionMask m_selection;
    RectSelectTool m_rectSelect;

    SelectionMoveSession m_moveSession;
    bool m_draggingMove = false;
    int m_moveStartX = 0;
    int m_moveStartY = 0;

    int m_phase = 0;
};
