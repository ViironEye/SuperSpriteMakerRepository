#pragma once
#include "SpriteEditor.h"
#include "GLTexturePresenter.h"
#include "Viewport.h"
#include "../../imgui/imgui.h"
#include "ToolAtlas.h"
#include "../tool/Pencil.h"
#include "../tool/Brush.h"
#include "../tool/InkTool.h"
#include "../tool/ShapeTool.h"

enum class ToolParamsPanel
{
    None,
    Ink,
    Brush,
    Eraser,
    Shape
};

class EditorUI
{
public:
    EditorUI(SpriteEditor* editor)
        : m_editor(editor) {
    }

    void draw();
    Viewport& viewport() { return m_vp; }

private:
    SpriteEditor* m_editor = nullptr;
    Viewport m_vp;

    GLTexturePresenter m_presenter;
    PresenterOptions m_presentOpt;

    PencilTool m_pencil{ PixelRGBA8(0,0,0,255) };
    InkTool m_ink{ PixelRGBA8(0,0,0,255) };
    BrushTool m_brush{ PixelRGBA8(0,0,0,255), BrushSettings() };

    ShapeSettings m_shapeSettings;
    ShapeTool m_shapeTool{ m_shapeSettings };

    int m_toolIndex = 0;
    float m_color[4] = { 0.f, 0.f, 0.f, 0.f };

    ToolAtlas m_toolAtlas;
    bool m_atlasReady = false;

    ToolParamsPanel m_paramsPanel = ToolParamsPanel::None;
};
