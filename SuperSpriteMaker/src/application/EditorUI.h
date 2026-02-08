#pragma once
#include "SpriteEditor.h"
#include "Viewport.h"
#include "../../imgui/imgui.h"

class EditorUI
{
public:
    EditorUI(SpriteEditor* editor)
        : m_editor(editor) {
    }

    void draw();                // рисует весь UI
    Viewport& viewport() { return m_vp; }

private:
    SpriteEditor* m_editor = nullptr;
    Viewport m_vp;

    // UI state
    int m_toolIndex = 0; // 0 pencil, 1 ink, 2 brush, 3 eraser, 4 select, 5 move
};
