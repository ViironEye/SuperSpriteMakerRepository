#include "EditorUI.h"
#include "../tool/BrushOverlay.h"
#include "../tool/Brush.h"
#include "../tool/SelectionOverlayUI.h"
#include "../tool/ShapeOverlay.h"
#include "../tool/SelectionOverlay.h"
#include "../tool/RectSelectTool.h"
#include "../structs/ShapeSettings.h"
#include "ImageImport.h"
#include "FileDialog.h"
#include "Utf.h"
#include "ImageWriter.h"
#include <iostream>

static Modifiers modsFromImGui()
{
    Modifiers m;
    ImGuiIO& io = ImGui::GetIO();
    m.shift = io.KeyShift;
    m.ctrl = io.KeyCtrl;
    m.alt = io.KeyAlt;
    return m;
}

static PixelRGBA8 toPixel(const float col[4])
{
    return PixelRGBA8(
        (uint8_t)(col[0] * 255.0f),
        (uint8_t)(col[1] * 255.0f),
        (uint8_t)(col[2] * 255.0f),
        (uint8_t)(col[3] * 255.0f)
    );
}

void EditorUI::draw()
{
    if (!m_editor) return;

    ImGui::Begin("Tools");

    static char s_path[260] = "export.png";
    static bool s_openExport = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Export PNG..."))
            {
                std::wstring wpath;
                if (FileDialog::savePNG(wpath))
                {
                    const PixelBuffer& pb = m_editor->compositePixels();
                    ImageWriter::savePNG(wideToUtf8(wpath), pb, /*flipY*/true);
                }
            }

            if (ImGui::MenuItem("Export JPEG..."))
            {
                std::wstring wpath;
                if (FileDialog::saveJPG(wpath))
                {
                    const PixelBuffer& pb = m_editor->compositePixels();
                    ImageWriter::saveJPG(wideToUtf8(wpath), pb, /*quality*/90, /*flipY*/true);
                }
            }

            if (ImGui::MenuItem("Import Image as Layer..."))
            {
                std::wstring wpath;
                if (FileDialog::openImage(wpath))
                {
                    std::string path = wideToUtf8(wpath);

                    // nearest=true (pixel art)
                    ImageImport::importAsNewLayer(*m_editor, path, true);
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (s_openExport)
        ImGui::OpenPopup("Export Image");

    if (ImGui::BeginPopupModal("Export Image", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("Path", s_path, IM_ARRAYSIZE(s_path));

        // проста€ логика выбора формата по расширению
        bool isPNG = false, isJPG = false;
        {
            std::string p = s_path;
            auto lower = [](char c) { return (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c; };
            for (auto& c : p) c = lower(c);
            isPNG = (p.size() >= 4 && p.substr(p.size() - 4) == ".png");
            isJPG = (p.size() >= 4 && (p.substr(p.size() - 4) == ".jpg" || p.substr(p.size() - 5) == ".jpeg"));
        }

        static int jpgQuality = 90;
        if (isJPG)
            ImGui::SliderInt("JPEG Quality", &jpgQuality, 1, 100);

        static bool flipY = true;
        ImGui::Checkbox("Flip Y", &flipY);

        if (ImGui::Button("Save", ImVec2(120, 0)))
        {
            const PixelBuffer& pb = m_editor->compositePixels();
            bool ok = false;

            if (isPNG)
                ok = ImageWriter::savePNG(s_path, pb, flipY);
            else if (isJPG)
                ok = ImageWriter::saveJPG(s_path, pb, jpgQuality, flipY);

            // ћожно вывести статус
            if (!ok)
                ImGui::OpenPopup("Export Failed");
            else
                ImGui::CloseCurrentPopup();

            s_openExport = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            s_openExport = false;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::BeginPopupModal("Export Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted("Failed to export image.\nCheck path/format (use .png/.jpg/.jpeg).");
            if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        ImGui::EndPopup();
    }

    if (!m_atlasReady)
        m_atlasReady = m_toolAtlas.create();

    const float btn = 32.0f;
    const ImVec2 size(btn, btn);

    const char* toolTips[] = {
    "Pencil (P)", "Ink (I)", "Brush (B)",
    "Eraser (E)", "Select (M)", "Move (V)",
    "Shapes (U)", "Fill (F)", "Eyedropper (A)"
    };

    bool shapeClicked = false;

    auto iconBtn = [&](int idx, const char* tooltip) -> bool
        {
            float u0, v0, u1, v1;
            m_toolAtlas.getUV(idx, u0, v0, u1, v1);

            const bool selected = (m_toolIndex == idx);

            ImGui::PushID(idx);

            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

            bool clicked = ImGui::ImageButton(
                "##icon",
                (ImTextureID)(uintptr_t)m_toolAtlas.texture(),
                ImVec2(32, 32),
                ImVec2(u0, v0), ImVec2(u1, v1)
            );

            if (selected)
                ImGui::PopStyleColor();

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", tooltip);

            // рамка активного
            if (selected) {
                ImVec2 a = ImGui::GetItemRectMin();
                ImVec2 b = ImGui::GetItemRectMax();
                ImGui::GetWindowDrawList()->AddRect(a, b, IM_COL32(255, 255, 255, 200), 0.0f, 0, 2.0f);
            }

            // ѕравый клик -> открыть параметры (если у инструмента есть)
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                if (idx == 1) m_paramsPanel = ToolParamsPanel::Ink;
                if (idx == 2) m_paramsPanel = ToolParamsPanel::Brush;
                if (idx == 3) m_paramsPanel = ToolParamsPanel::Eraser;
                if (idx == 6) m_paramsPanel = ToolParamsPanel::Shape;
                if (idx == 7) m_paramsPanel = ToolParamsPanel::Fill;
            }

            // Ћевый клик: выбрать инструмент; если уже выбран Ч открыть параметры
            if (clicked)
            {
                if (!selected)
                {
                    m_toolIndex = idx;
                }
                else
                {
                    // повторный клик по выбранному открывает параметры
                    if (idx == 1) m_paramsPanel = ToolParamsPanel::Ink;
                    if (idx == 2) m_paramsPanel = ToolParamsPanel::Brush;
                    if (idx == 3) m_paramsPanel = ToolParamsPanel::Eraser;
                    if (idx == 6) m_paramsPanel = ToolParamsPanel::Shape;
                    if (idx == 7) m_paramsPanel = ToolParamsPanel::Fill;
                }
            }

            ImGui::PopID();
            return clicked;
        };

    //Tools
    ImGui::BeginChild("ToolsArea", ImVec2(160.f, 140.f), true, ImGuiWindowFlags_NoScrollbar);
    iconBtn(0, "Pencil"); ImGui::SameLine(); iconBtn(1, "Ink"); ImGui::SameLine(); iconBtn(2, "Brush");
    iconBtn(3, "Eraser"); ImGui::SameLine(); iconBtn(4, "Select"); ImGui::SameLine(); iconBtn(5, "Move");
    iconBtn(6, "Shapes"); ImGui::SameLine(); iconBtn(7, "Fill"); ImGui::SameLine(); iconBtn(8, "Eyedropper");

    PixelRGBA8 currentColor = toPixel(m_color);

    switch (m_toolIndex)
    {
    case 0:
        m_editor->setMode(EditorMode::Draw);
        m_pencil.setColor(currentColor);
        m_editor->setTool(&m_pencil);
        m_paramsPanel = ToolParamsPanel::None;
        break;

    case 1:
        m_editor->setMode(EditorMode::Draw);
        m_ink.setColor(currentColor);
        m_editor->setTool(&m_ink);
        m_paramsPanel = ToolParamsPanel::Ink;
        break;

    case 2:
        m_editor->setMode(EditorMode::Draw);
        m_brush.setEraser(false);
        m_brush.setColor(currentColor);
        m_editor->setTool(&m_brush);
        m_paramsPanel = ToolParamsPanel::Brush;
        break;

    case 3:
        m_editor->setMode(EditorMode::Draw);
        m_brush.setEraser(true);
        m_editor->setTool(&m_brush);
        m_paramsPanel = ToolParamsPanel::Eraser;
        break;

    case 4:
        m_editor->setMode(EditorMode::SelectRect);
        break;

    case 5:
        m_editor->setMode(EditorMode::MoveSelect);
        break;

    case 6:
        m_editor->setMode(EditorMode::Draw);
        m_shapeTool.setColor(currentColor);
        m_shapeTool.setSettings(m_shapeSettings);
        m_editor->setTool(&m_shapeTool);
        m_paramsPanel = ToolParamsPanel::Shape;
        break;

    case 7:
        m_editor->setMode(EditorMode::Draw);
        m_fillTool.setColor(currentColor);
        m_editor->setTool(&m_fillTool);
        m_paramsPanel = ToolParamsPanel::Fill;
        break;
    case 8:
        m_editor->setMode(EditorMode::Draw);
        m_editor->setTool(&m_dropper);
        m_paramsPanel = ToolParamsPanel::Eyedropper;
        break;
    }

    if (m_toolIndex == 5)
    {
        ImGui::TextUnformatted("Move");
        ImGui::Separator();

        MoveMode mm = m_editor->moveModeDefault();
        int mode = (mm == MoveMode::Cut) ? 0 : 1;

        const bool moving = m_editor->moveSession().active();
        if (moving)
            ImGui::BeginDisabled();

        if (ImGui::RadioButton("Cut", mode == 0)) mode = 0;
        if (ImGui::RadioButton("Copy", mode == 1)) mode = 1;

        if (moving)
            ImGui::EndDisabled();

        m_editor->setMoveModeDefault(mode == 0 ? MoveMode::Cut : MoveMode::Copy);

        ImGui::TextUnformatted("Tip: hold Alt to Copy (temporary).");
    }

    if (shapeClicked)
    {
        if (m_toolIndex == 6)
        {
            m_paramsPanel = ToolParamsPanel::Shape;
            ImGui::OpenPopup("ShapeModePopup");
        }
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        m_toolIndex = 6;
        m_paramsPanel = ToolParamsPanel::Shape;
        ImGui::OpenPopup("ShapeModePopup");
    }

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
    ImGui::EndChild();

    //Colors
    PixelRGBA8 pc = m_editor->primaryColor();
    m_color[0] = pc.r / 255.0f;
    m_color[1] = pc.g / 255.0f;
    m_color[2] = pc.b / 255.0f;
    m_color[3] = pc.a / 255.0f;

    ImGui::SameLine();
    ImGui::BeginChild("ColorArea", ImVec2(0.f, 140.f), true, ImGuiWindowFlags_NoScrollbar);
    if (ImGui::ColorButton("FG",
        ImVec4(m_color[0], m_color[1], m_color[2], m_color[3]),
        ImGuiColorEditFlags_NoTooltip,
        ImVec2(28, 28)))
    {
        ImGui::OpenPopup("ColorPopup");
    }

    if (ImGui::BeginPopup("ColorPopup"))
    {
        ImGui::ColorPicker4("##picker", m_color,
            ImGuiColorEditFlags_DisplayRGB |
            ImGuiColorEditFlags_AlphaBar);
        ImGui::EndPopup();
    }

    PixelRGBA8 newC = toPixel(m_color);
    m_editor->setPrimaryColor(newC);

    ImGui::Separator();
    ImGui::TextUnformatted("Tool Properties");
    ImGui::Separator();

    switch (m_paramsPanel)
    {
    case ToolParamsPanel::Ink:
    {
        InkSettings& s = m_ink.settings();

        ImGui::TextUnformatted("Ink");
        ImGui::SliderFloat("Size", &s.size, 1.0f, 64.0f, "%.1f");
        ImGui::SliderFloat("Opacity", &s.opacity, 0.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Opacity Pressure", &s.opacityPressure);
        break;
    }
    case ToolParamsPanel::Brush:
    {
        BrushSettings& s = m_brush.settings();

        ImGui::TextUnformatted("Brush");
        ImGui::SliderFloat("Radius", &s.radius, 1.0f, 128.0f, "%.1f");
        ImGui::SliderFloat("Spacing", &s.spacing, 0.01f, 1.0f, "%.2f");
        ImGui::SliderFloat("Opacity", &s.opacity, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Hardness", &s.hardness, 0.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Size Pressure", &s.sizePressure);
        ImGui::Checkbox("Opacity Pressure", &s.opacityPressure);

        // нормализаци€
        if (s.radius < 1.0f) s.radius = 1.0f;
        if (s.spacing < 0.01f) s.spacing = 0.01f;
        if (s.hardness < 0.0f) s.hardness = 0.0f;
        if (s.hardness > 1.0f) s.hardness = 1.0f;
        break;
    }
    case ToolParamsPanel::Eraser:
    {
        BrushSettings& s = m_brush.settings();

        ImGui::TextUnformatted("Eraser");
        ImGui::SliderFloat("Radius", &s.radius, 1.0f, 128.0f, "%.1f");
        ImGui::SliderFloat("Spacing", &s.spacing, 0.01f, 1.0f, "%.2f");
        ImGui::SliderFloat("Opacity", &s.opacity, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Hardness", &s.hardness, 0.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Size Pressure", &s.sizePressure);
        ImGui::Checkbox("Opacity Pressure", &s.opacityPressure);
        break;
    }
    case ToolParamsPanel::Shape:
    {
        ImGui::TextUnformatted("Shape");
        ImGui::Separator();

        int mode = (int)m_shapeSettings.mode;
        const char* modes[] = { "Rectangle", "Ellipse", "Line" };
        ImGui::Text("Mode: %s", modes[mode]);

        if (ImGui::Button("Change Mode..."))
            ImGui::OpenPopup("ShapeModePopup");

        // можно продублировать быстрый выбор пр€мо здесь:
        if (ImGui::RadioButton("Rectangle##shape", mode == 0)) mode = 0;
        if (ImGui::RadioButton("Ellipse##shape", mode == 1)) mode = 1;
        if (ImGui::RadioButton("Line##shape", mode == 2)) mode = 2;

        if (mode != 2)
            ImGui::Checkbox("Filled", &m_shapeSettings.filled);
        else
            m_shapeSettings.filled = false;

        ImGui::SliderInt("Thickness", &m_shapeSettings.thickness, 1, 16);

        m_shapeSettings.mode = (ShapeMode)mode;
        m_shapeTool.setSettings(m_shapeSettings);

        // сам popup (общий) может жить тут же Ч он глобален в рамках окна
        if (ImGui::BeginPopup("ShapeModePopup"))
        {
            int pmode = (int)m_shapeSettings.mode;

            ImGui::TextUnformatted("Shape Mode");
            ImGui::Separator();

            if (ImGui::RadioButton("Rectangle", pmode == 0)) pmode = 0;
            if (ImGui::RadioButton("Ellipse", pmode == 1)) pmode = 1;
            if (ImGui::RadioButton("Line", pmode == 2)) pmode = 2;

            ImGui::Separator();
            if (pmode != 2)
                ImGui::Checkbox("Filled", &m_shapeSettings.filled);
            else
                m_shapeSettings.filled = false;

            ImGui::SliderInt("Thickness", &m_shapeSettings.thickness, 1, 16);

            m_shapeSettings.mode = (ShapeMode)pmode;
            m_shapeTool.setSettings(m_shapeSettings);

            ImGui::EndPopup();
        }

        break;
    }
    case ToolParamsPanel::Fill:
    {
        FillSettings& s = m_fillTool.settings();
        ImGui::TextUnformatted("Fill");
        ImGui::SliderInt("Tolerance", &s.tolerance, 0, 255);
        ImGui::Checkbox("Contiguous", &s.contiguous);
        ImGui::Checkbox("Include Alpha", &s.includeAlpha);
        break;
    }
    case ToolParamsPanel::Eyedropper:
    {
        EyedropperSettings& d = m_dropper.settings();
        ImGui::TextUnformatted("Eyedropper");
        ImGui::Checkbox("Sample Merged", &d.sampleMerged);
        break;
    }
    default:
        ImGui::TextUnformatted("Select Ink/Brush/Eraser to edit settings.");
        break;
    }

    ImGui::EndChild();
    //
    ImGui::Separator();
    if (ImGui::Button("Undo")) m_editor->undo().undo();
    ImGui::SameLine();
    if (ImGui::Button("Redo")) m_editor->undo().redo();

    // ---------- Viewport ----------
    ImGuiWindowFlags canvasFlags =
        ImGuiWindowFlags_NoScrollbar;// | ImGuiWindowFlags_NoScrollWithMouse;

    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    if (canvasSize.x < 1) canvasSize.x = 1;
    if (canvasSize.y < 1) canvasSize.y = 1;

    ImGui::BeginChild("CanvasArea", canvasSize, true, canvasFlags);

    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 avail = ImGui::GetContentRegionAvail();
    if (avail.x < 1) avail.x = 1;
    if (avail.y < 1) avail.y = 1;

    m_vp.setViewRect(p0.x, p0.y, avail.x, avail.y);

    auto* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(
        ImVec2(m_vp.originX, m_vp.originY),
        ImVec2(m_vp.originX + m_vp.viewW, m_vp.originY + m_vp.viewH),
        IM_COL32(50, 50, 60, 255)
    );
    dl->AddRect(
        ImVec2(m_vp.originX, m_vp.originY),
        ImVec2(m_vp.originX + m_vp.viewW, m_vp.originY + m_vp.viewH),
        IM_COL32(255, 255, 255, 255)
    );

    ImGui::InvisibleButton("canvas_btn", avail,
        ImGuiButtonFlags_MouseButtonLeft |
        ImGuiButtonFlags_MouseButtonRight |
        ImGuiButtonFlags_MouseButtonMiddle);

    ImGuiIO& io = ImGui::GetIO();
    const bool hovered = ImGui::IsItemHovered();

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
            ImVec2 d(io.MousePos.x - panStart.x, io.MousePos.y - panStart.y);
            m_vp.panX = panX0 + d.x;
            m_vp.panY = panY0 + d.y;
        }
        else {
            panning = false;
        }
    }
    if (hovered)
    {
        int cx, cy;
        if (m_vp.screenToCanvas(io.MousePos.x, io.MousePos.y, cx, cy))
        {
            float pressure = 1.0f;
            Modifiers mods;
            mods.shift = io.KeyShift;
            mods.ctrl = io.KeyCtrl;
            mods.alt = io.KeyAlt;

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                m_editor->pointerDown(cx, cy, pressure, mods);
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                m_editor->pointerMove(cx, cy, pressure, mods);
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                m_editor->pointerUp(cx, cy, pressure, mods);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) m_editor->keyEsc();
        if (ImGui::IsKeyPressed(ImGuiKey_Enter))  m_editor->keyEnter();
    }

    const PixelBuffer& pb = m_editor->compositePixels();

    if (&pb != nullptr)
    {
        m_presentOpt.flipY = false;
        m_presentOpt.checkerboard = true;
        m_presentOpt.grid = true;
        m_presentOpt.gridMinZoom = 8;
        m_presentOpt.outline = true;

        m_presenter.present(pb, m_vp, m_presentOpt);

        // preview раст€гивани€ пр€моугольного выделени€
        if (m_editor->mode() == EditorMode::SelectRect)
        {
            const RectSelectTool& rs = m_editor->rectSelectTool(); // нужен геттер (см. ниже)
            if (rs.active())
            {
                SelectionOverlay::drawRubberBand(
                    m_vp,
                    rs.ax(), rs.ay(),
                    rs.bx(), rs.by(),
                    rs.op()
                );
            }
        }


        if (hovered)
        {
            int cx, cy;
            if (m_vp.screenToCanvas(io.MousePos.x, io.MousePos.y, cx, cy))
            {
                Tool* t = m_editor->tool();
                if (auto* brush = dynamic_cast<BrushTool*>(t))
                    BrushOverlay::drawBrushCircle(m_vp, cx, cy, brush->settings(), 1.0f, true);
            }
        }

        Sprite* spr = m_editor->sprite();
        if (spr)
        {
            if (ImGui::Button("+ New Layer"))
            {
                int n = spr->layerCount() + 1;
                spr->createLayer("Layer " + std::to_string(n));
                m_editor->setActiveLayerIndex(spr->layerCount() - 1);
            }

            ImGui::Separator();

            for (int i = spr->layerCount() - 1; i >= 0; --i)
            {
                Layer* L = spr->getLayer(i);
                if (!L) continue;

                ImGui::PushID(i);

                bool selected = (m_editor->activeLayerIndex() == i);

                bool vis = L->visible();
                if (ImGui::Checkbox("##vis", &vis))
                    L->setVisible(vis);

                ImGui::SameLine();

                if (ImGui::Selectable(L->name().c_str(), selected))
                    m_editor->setActiveLayerIndex(i);

                if (selected)
                {
                    int opPct = (int)std::lround(L->opacity() * 100.0f / 255.0f);
                    if (ImGui::SliderInt("Opacity", &opPct, 0, 100))
                    {
                        int o = (opPct * 255) / 100;
                        if (o < 0) o = 0; if (o > 255) o = 255;
                        L->setOpacity((uint8_t)o);
                    }
                }

                ImGui::PopID();
            }
        }

        if (m_editor->moveSession().active())
            SelectionOverlayUI::drawMoveOutline(m_vp, m_editor->moveSession(), true);
        else
            SelectionOverlayUI::drawSelectionOutline(m_vp, m_editor->selection(), true);

        Tool* t = m_editor->tool();
        if (auto* shape = dynamic_cast<ShapeTool*>(t))
        {
            if (shape->isActive())
            {
                ShapeOverlay::drawPreview(
                    m_vp,
                    shape->x0(), shape->y0(),
                    shape->x1(), shape->y1(),
                    shape->settings(),
                    shape->color()
                );
            }
        }

    }
    else
    {
        ImGui::Text("No active frame");
    }

    m_editor->tick();

    ImGui::EndChild();
    ImGui::End();
}
