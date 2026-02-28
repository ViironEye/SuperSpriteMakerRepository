#pragma once
#include <cstdint>
#include <glad/glad.h>

class PixelBuffer;

class GLTextureCache
{
public:
    GLTextureCache() = default;
    ~GLTextureCache();

    GLTextureCache(const GLTextureCache&) = delete;
    GLTextureCache& operator=(const GLTextureCache&) = delete;

    GLuint texture() const { return m_tex; }

    void ensure(int width, int height);
    void upload(const PixelBuffer& pb);

    void* imguiID() const { return (void*)(intptr_t)m_tex; }

    int width()  const { return m_w; }
    int height() const { return m_h; }

private:
    void create();
    void destroy();
    void allocate(int w, int h);

    GLuint m_tex = 0;
    int m_w = 0;
    int m_h = 0;
};
