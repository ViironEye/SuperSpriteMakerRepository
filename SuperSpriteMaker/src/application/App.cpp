#include "App.h"

#include <cstdio>
#include <utility>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <../../imgui/imgui.h>
#include <../../imgui/backends/imgui_impl_glfw.h>
#include <../../imgui/backends/imgui_impl_opengl3.h>

#include "../model/Sprite.h"
#include "SpriteEditor.h"
#include "EditorUI.h"

#include "../tool/Pencil.h"
#include "../tool/InkTool.h"
#include "../tool/Brush.h"
#include "../tool/ShapeTool.h"

static void glfw_error_callback(int error, const char* description)
{
    std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

App::~App()
{
    shutdown();
}

bool App::init(int width, int height, const char* title)
{
    if (m_running)
        return true;

    if (!initGLFW(width, height, title))
        return false;

    if (!initGLAD())
        return false;

    if (!initImGui())
        return false;

    m_showNewSpriteDialog = true;
    m_running = true;
    return true;
}

void App::render()
{
    if (m_showNewSpriteDialog)
    {
        showNewSpriteDialog();
    }

    if (!m_showNewSpriteDialog && m_ui)
        m_ui->draw();
}

void App::showNewSpriteDialog()
{
    ImGui::OpenPopup("New Sprite");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("New Sprite", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Create New Sprite");
        ImGui::Separator();

        ImGui::InputInt("Width", &m_newWidth);
        ImGui::InputInt("Height", &m_newHeight);

        if (m_newWidth < 1)  m_newWidth = 1;
        if (m_newHeight < 1) m_newHeight = 1;
        if (m_newWidth > INT32_MAX) m_newWidth = INT32_MAX;
        if (m_newHeight > INT32_MAX) m_newHeight = INT32_MAX;

        ImGui::Spacing();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            createDocument(m_newWidth, m_newHeight);
            m_showNewSpriteDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Exit", ImVec2(120, 0)))
        {
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        }

        ImGui::EndPopup();
    }
}

int App::run()
{
    if (!m_running) {
        if (!init())
            return 1;
    }

    while (!glfwWindowShouldClose(m_window))
    {
        frameBegin();
        frameRender();
        frameEnd();
    }

    shutdown();
    return 0;
}

void App::shutdown()
{
    if (!m_window && !m_running)
        return;

    m_ui.reset();
    m_editor.reset();
    m_sprite.reset();

    shutdownImGui();
    shutdownGLFW();

    m_running = false;
}

bool App::initGLFW(int width, int height, const char* title)
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to init GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        std::fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    return true;
}

bool App::initGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to init GLAD\n");
        return false;
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

bool App::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)) {
        std::fprintf(stderr, "ImGui_ImplGlfw_InitForOpenGL failed\n");
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        std::fprintf(stderr, "ImGui_ImplOpenGL3_Init failed\n");
        return false;
    }

    return true;
}

void App::shutdownImGui()
{
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void App::shutdownGLFW()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void App::frameBegin()
{
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void App::frameRender()
{
    render();

    ImGui::Render();

    int fbW = 0, fbH = 0;
    glfwGetFramebufferSize(m_window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);

    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::frameEnd()
{
    glfwSwapBuffers(m_window);
}

void App::createDocument(int w, int h)
{
    m_sprite = std::make_unique<Sprite>(w, h, PixelFormat::RGBA8);

    Frame* f0 = m_sprite->createFrame(100);
    (void)f0;

    Layer* l0 = m_sprite->createLayer("Layer 1");

    l0->addCel(0, new Cel(m_sprite->getFrame(0)));
    m_editor = std::make_unique<SpriteEditor>(m_sprite.get());
    m_editor->setActiveFrameIndex(0);

    m_ui = std::make_unique<EditorUI>(m_editor.get());

    static PencilTool s_pencil(PixelRGBA8(0, 0, 0, 255));
    m_editor->setTool(&s_pencil);
}