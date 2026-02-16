#pragma once
#include "SpriteEditor.h"
#include "GLTexturePresenter.h"
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

    GLTexturePresenter m_presenter;
    PresenterOptions m_presentOpt;

    int m_toolIndex = 0;
};
