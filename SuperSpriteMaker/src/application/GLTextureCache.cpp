#include "GLTextureCache.h"
#include "../model/PixelBuffer.h"

GLTextureCache::~GLTextureCache()
{
    destroy();
}

void GLTextureCache::create()
{
    if (m_tex != 0) return;

    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);

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
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
    ensure(pb.width(), pb.height());
    if (m_tex == 0) return;

    pb.uploadToGLTexture(m_tex);
}
