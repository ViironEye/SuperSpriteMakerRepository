#include "EditorUI.h"

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

    // ---------- Панель инструментов ----------
    ImGui::Begin("Tools");

    const char* tools[] = { "Pencil", "Ink", "Brush", "Eraser", "Rect Select", "Move" };
    ImGui::ListBox("Tool", &m_toolIndex, tools, IM_ARRAYSIZE(tools), 6);

    // Простейшее переключение режимов (инструменты ты уже создал отдельно и передаёшь сюда)
    if (m_toolIndex <= 3) {
        m_editor->setMode(EditorMode::Draw);
    }
    else if (m_toolIndex == 4) {
        m_editor->setMode(EditorMode::SelectRect);
    }
    else {
        m_editor->setMode(EditorMode::MoveSelect);
    }

    if (ImGui::Button("Undo")) m_editor->undo().undo();
    ImGui::SameLine();
    if (ImGui::Button("Redo")) m_editor->undo().redo();

    ImGui::Text("Mode: %d", (int)m_editor->mode());
    ImGui::End();

    // ---------- Viewport ----------
    ImGui::Begin("Viewport");

    // получаем прямоугольник области для рисования
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 avail = ImGui::GetContentRegionAvail();

    if (avail.x < 1) avail.x = 1;
    if (avail.y < 1) avail.y = 1;

    m_vp.setViewRect(p0.x, p0.y, avail.x, avail.y);

    // Создаем "кликабельную" область
    ImGui::InvisibleButton("canvas", avail,
        ImGuiButtonFlags_MouseButtonLeft |
        ImGuiButtonFlags_MouseButtonRight |
        ImGuiButtonFlags_MouseButtonMiddle);

    ImGuiIO& io = ImGui::GetIO();
    const bool hovered = ImGui::IsItemHovered();
    const bool active = ImGui::IsItemActive();

    // zoom колесом мыши, pan средней кнопкой
    if (hovered && io.MouseWheel != 0.0f) {
        float factor = (io.MouseWheel > 0) ? 1.1f : (1.0f / 1.1f);
        m_vp.zoomAt(factor, io.MousePos.x, io.MousePos.y);
    }

    static bool panning = false;
    static ImVec2 panStart{};
    static float panX0 = 0, panY0 = 0;

    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
        panning = true;
        panStart = io.MousePos;
        panX0 = m_vp.panX;
        panY0 = m_vp.panY;
    }
    if (panning) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
            ImVec2 d = ImVec2(io.MousePos.x - panStart.x, io.MousePos.y - panStart.y);
            m_vp.panX = panX0 + d.x;
            m_vp.panY = panY0 + d.y;
        }
        else {
            panning = false;
        }
    }

    // ----- input -> SpriteEditor -----
    if (hovered) {
        int cx, cy;
        if (m_vp.screenToCanvas(io.MousePos.x, io.MousePos.y, cx, cy)) {
            float pressure = 1.0f; // мышь; позже заменишь на tablet pressure

            Modifiers mods = modsFromImGui();

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                m_editor->pointerDown(cx, cy, pressure, mods);

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                m_editor->pointerMove(cx, cy, pressure, mods);

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                m_editor->pointerUp(cx, cy, pressure, mods);
        }

        // commit/cancel move
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) m_editor->keyEsc();
        if (ImGui::IsKeyPressed(ImGuiKey_Enter))  m_editor->keyEnter();
    }

    // marching ants animation tick
    m_editor->tick();

    // Тут позже будет отрисовка:
    // 1) canvas.renderFrame(...)
    // 2) uploadToGLTexture
    // 3) ImGui::Image(texture, ...)
    ImGui::Text("Render hook goes here (texture upload + ImGui::Image)");

    ImGui::End();
}
