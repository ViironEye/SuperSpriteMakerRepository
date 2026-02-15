#pragma once
#pragma once
#include <memory>
#include <cstdint>

#include "../model/Sprite.h"
#include "../model/Frame.h"
#include "../tool/UndoStack.h"
#include "../tool/Stroke.h"      // твой Stroke
#include "../tool/SelectionMask.h"
#include "../tool/SelectionMoveSession.h"
#include "../tool/RectSelectTool.h"
#include "../tool/Tool.h"

// Какие действия сейчас активны
enum class EditorMode {
    Draw,       // Pencil/Ink/Brush/Eraser
    SelectRect, // Rect select
    MoveSelect  // Move selection
};

// Состояние модификаторов клавиатуры
struct Modifiers {
    bool shift = false;
    bool ctrl = false;
    bool alt = false;
};

// Простейший контроллер: хранит активные объекты и обрабатывает input.
class SpriteEditor {
public:
    explicit SpriteEditor(Sprite* sprite)
        : m_sprite(sprite)
    {
        if (m_sprite)
            m_selection.resize(m_sprite->width(), m_sprite->height());
    }

    // ====== Active context ======
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

    // ====== Input API (UI вызывает это) ======
    void pointerDown(int x, int y, float pressure, const Modifiers& mods);
    void pointerMove(int x, int y, float pressure, const Modifiers& mods);
    void pointerUp(int x, int y, float pressure, const Modifiers& mods);

    void keyEsc();   // cancel move
    void keyEnter(); // commit move

    // Для marching ants
    int selectionPhase() const { return m_phase; }
    void tick() { ++m_phase; } // вызывать раз в кадр

    SelectionMoveSession& moveSession() { return m_moveSession; }
    const SelectionMoveSession& moveSession() const { return m_moveSession; }

private:
    SelectionOp selectionOpFromMods(const Modifiers& mods) const;

private:
    Sprite* m_sprite = nullptr;
    int m_activeFrame = 0;

    EditorMode m_mode = EditorMode::Draw;
    Tool* m_tool = nullptr;

    // Undo
    UndoStack m_undo;

    // Drawing stroke
    std::unique_ptr<StrokeCommand> m_strokeCmd;
    std::unique_ptr<Stroke> m_stroke; // твой Stroke(Tool*, Frame*, StrokeCommand*)

    // Selection
    SelectionMask m_selection;
    RectSelectTool m_rectSelect;

    // Move selection
    SelectionMoveSession m_moveSession;
    bool m_draggingMove = false;
    int m_moveStartX = 0;
    int m_moveStartY = 0;

    // marching ants phase
    int m_phase = 0;
};
