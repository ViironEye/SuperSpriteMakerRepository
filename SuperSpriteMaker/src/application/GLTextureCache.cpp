#include "GLTextureCache.h"
#include "../model/PixelBuffer.h" // твой класс

GLTextureCache::~GLTextureCache()
{
    destroy();
}

void GLTextureCache::create()
{
    if (m_tex != 0)
        return;

    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);

    // Пиксель-арт дефолты
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTextureCache::destroy()
{
    if (m_tex != 0) {
        glDeleteTextures(1, &m_tex);
        m_tex = 0;
    }
    m_w = 0;
    m_h = 0;
}

void GLTextureCache::allocate(int w, int h)
{
    // Выделяем/перевыделяем память текстуры под RGBA8
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // ВАЖНО: выделяем, но не загружаем данные (nullptr)
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        w, h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_w = w;
    m_h = h;
}

void GLTextureCache::ensure(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;

    create();

    if (m_w != width || m_h != height)
        allocate(width, height);
}

void GLTextureCache::upload(const PixelBuffer& pb)
{
    // Пока предполагаем, что pb в RGBA8 (как твой Canvas/preview).
    // Если позже добавишь другие форматы, здесь будет конвертация в RGBA8 временным буфером.

    ensure(pb.width(), pb.height());
    if (m_tex == 0)
        return;

    // У тебя уже есть готовая функция — используем её.
    // Она должна делать glBindTexture + glTexSubImage2D (или glTexImage2D).
    pb.uploadToGLTexture(m_tex);
}
