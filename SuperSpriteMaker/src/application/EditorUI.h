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
#include "../tool/RectSelectTool.h"
#include "../tool/SelectionMoveSession.h"
#include "../tool/FillTool.h"
#include "../tool/Eyedropper.h"

enum class ToolParamsPanel
{
    None,
    Ink,
    Brush,
    Eraser,
    Shape,
    Fill,
    Eyedropper
};

class EditorUI
{
public:
    EditorUI(SpriteEditor* editor) : m_editor(editor) { }

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

    RectSelectTool m_selectTool;
    SelectionMoveSession m_moveTool;

    FillTool m_fillTool;
    EyedropperTool m_dropper{ m_editor };

    int m_toolIndex = 0;
    float m_color[4] = { 0.f, 0.f, 0.f, 0.f };

    ToolAtlas m_toolAtlas;
    bool m_atlasReady = false;

    ToolParamsPanel m_paramsPanel = ToolParamsPanel::None;
};
