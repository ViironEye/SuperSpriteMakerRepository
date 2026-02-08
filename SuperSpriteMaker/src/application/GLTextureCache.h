#pragma once
#include <cstdint>
#include <glad/glad.h>

class PixelBuffer;

// Кэш одной текстуры (для viewport/preview)
class GLTextureCache
{
public:
    GLTextureCache() = default;
    ~GLTextureCache();

    GLTextureCache(const GLTextureCache&) = delete;
    GLTextureCache& operator=(const GLTextureCache&) = delete;

    // Возвращает OpenGL texture id (создаёт при необходимости)
    GLuint texture() const { return m_tex; }

    // Убедиться, что текстура существует и соответствует нужным параметрам
    void ensure(int width, int height);

    // Полностью обновить содержимое из PixelBuffer
    void upload(const PixelBuffer& pb);

    // Для ImGui::Image
    void* imguiID() const { return (void*)(intptr_t)m_tex; }

    int width()  const { return m_w; }
    int height() const { return m_h; }

private:
    void create();
    void destroy();
    void allocate(int w, int h);

private:
    GLuint m_tex = 0;
    int m_w = 0;
    int m_h = 0;
};
