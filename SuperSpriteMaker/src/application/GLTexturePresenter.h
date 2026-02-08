#pragma once
#include <glad/glad.h>
#include "GLTextureCache.h"

struct Viewport;
class PixelBuffer;

struct PresenterOptions
{
    bool flipY = true;

    bool checkerboard = true;
    int  checkerCellCanvasPx = 8;  // размер клетки в canvas-пикселях

    bool grid = true;
    int  gridMinZoom = 8;          // показывать сетку если zoom >= N (screen px per canvas px)

    bool outline = true;           // тонкая рамка по границам изображения
};

class GLTexturePresenter
{
public:
    bool present(const PixelBuffer& pb,
        const Viewport& vp,
        const PresenterOptions& opt);

    GLuint texture() const { return m_cache.texture(); }
    void* imguiID() const { return m_cache.imguiID(); }

private:
    GLTextureCache m_cache;

    void drawCheckerboard(const Viewport& vp, int imgW, int imgH, int cellCanvasPx) const;
    void drawGrid(const Viewport& vp, int imgW, int imgH) const;
    void drawOutline(const Viewport& vp, int imgW, int imgH) const;
};
