#pragma once

#include <memory>
#include <string>

#include "EditorUI.h"

struct GLFWwindow;

class Sprite;
class SpriteEditor;
//class EditorUI;

class App
{
public:
    App() = default;
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    // Создаёт окно, GL, ImGui и минимальный документ
    bool init(int width = 1280, int height = 720, const char* title = "SuperSpriteMaker");
    int  run();
    void shutdown();

    GLFWwindow* window() const { return m_window; }

private:
    bool initGLFW(int width, int height, const char* title);
    bool initGLAD();
    bool initImGui();
    void shutdownImGui();
    void shutdownGLFW();

    void frameBegin();
    void frameRender();
    void frameEnd();

    void createDocument();

private:
    GLFWwindow* m_window = nullptr;
    bool m_running = false;

    std::unique_ptr<Sprite> m_sprite;
    std::unique_ptr<SpriteEditor> m_editor;
    std::unique_ptr<EditorUI> m_ui;
};
