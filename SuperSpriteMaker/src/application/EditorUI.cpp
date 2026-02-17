#include "EditorUI.h"
#include "../tool/BrushOverlay.h"
#include "../tool/Brush.h"
#include "../tool/SelectionOverlayUI.h"

static Modifiers modsFromImGui()
{
    Modifiers m;
    ImGuiIO& io = ImGui::GetIO();
    m.shift = io.KeyShift;
    m.ctrl = io.KeyCtrl;
    m.alt = io.KeyAlt;
    return m;
}

void EditorUI::draw()
{
    if (!m_editor) return;

    ImGui::Begin("Tools");

    if (!m_atlasReady)
        m_atlasReady = m_toolAtlas.create();

    const float btn = 32.0f;
    const ImVec2 size(btn, btn);

    const char* toolTips[] = {
    "Pencil (P)", "Ink (I)", "Brush (B)",
    "Eraser (E)", "Select (M)", "Move (V)",
    "Shapes (U)"  // hotkey можно любая
    };

    auto iconBtn = [&](int idx) {
        float u0, v0, u1, v1;
        m_toolAtlas.getUV(idx, u0, v0, u1, v1);

        bool selected = (m_toolIndex == idx);
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

        ImGui::PushID(idx);

        bool clicked = ImGui::ImageButton(
            "##icon",
            (ImTextureID)(intptr_t)m_toolAtlas.texture(),
            ImVec2(32, 32),
            ImVec2(u0, v0),
            ImVec2(u1, v1)
        );

        ImGui::PopID();

        if (selected)
            ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", toolTips[idx]);

        if (selected) {
            ImVec2 a = ImGui::GetItemRectMin();
            ImVec2 b = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(a, b, IM_COL32(255, 255, 255, 200), 0.0f, 0, 2.0f);
        }

        if (clicked)
            m_toolIndex = idx;

        return clicked;
        };
    iconBtn(0); ImGui::SameLine(); iconBtn(1); ImGui::SameLine(); iconBtn(2);
    iconBtn(3); ImGui::SameLine(); iconBtn(4); ImGui::SameLine(); iconBtn(5);
    iconBtn(6);

    bool shapeClicked = false;

    switch (m_toolIndex)
    {
    case 0:
        m_editor->setMode(EditorMode::Draw);
        m_editor->setTool(&m_pencil);
        break;
    case 1:
        m_editor->setMode(EditorMode::Draw);
        m_editor->setTool(&m_ink);
        break;
    case 2:
        m_editor->setMode(EditorMode::Draw);
        m_brush.setEraser(false);
        m_editor->setTool(&m_brush);
        break;
    case 3:
        m_editor->setMode(EditorMode::Draw);
        m_brush.setEraser(true);
        m_editor->setTool(&m_brush);
        break;
    case 4:
        m_editor->setMode(EditorMode::SelectRect);
        break;
    case 5:
        m_editor->setMode(EditorMode::MoveSelect);
        break;
    case 6:
        m_editor->setMode(EditorMode::Draw);
        m_shapeTool.setSettings(m_shapeSettings);
        m_editor->setTool(&m_shapeTool);
        break;
    }

    shapeClicked = iconBtn(6);
    if (shapeClicked) ImGui::OpenPopup("ShapeModePopup");

    if (ImGui::BeginPopup("ShapeModePopup"))
    {
        int mode = (int)m_shapeSettings.mode;

        ImGui::TextUnformatted("Shape Mode");
        ImGui::Separator();

        if (ImGui::RadioButton("Rectangle", mode == 0)) mode = 0;
        if (ImGui::RadioButton("Ellipse", mode == 1)) mode = 1;
        if (ImGui::RadioButton("Line", mode == 2)) mode = 2;

        ImGui::Separator();
        ImGui::Checkbox("Filled", &m_shapeSettings.filled);
        ImGui::SliderInt("Thickness", &m_shapeSettings.thickness, 1, 16);

        m_shapeSettings.mode = (ShapeMode)mode;

        m_shapeTool.setSettings(m_shapeSettings);

        ImGui::EndPopup();
    }

    ImGui::Separator();
    if (ImGui::Button("Undo")) m_editor->undo().undo();
    ImGui::SameLine();
    if (ImGui::Button("Redo")) m_editor->undo().redo();

    ImGui::End();

}
