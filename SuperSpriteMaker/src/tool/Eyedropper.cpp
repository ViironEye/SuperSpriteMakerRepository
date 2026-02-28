#include "Eyedropper.h"
#include "../application/SpriteEditor.h"
#include "../model/Frame.h"
#include "../model/PixelBuffer.h"

void EyedropperTool::apply(Frame* frame, StrokeCommand* /*cmd*/, int x, int y, float /*pressure*/)
{
    if (!m_editor) return;

    PixelRGBA8 c(0, 0, 0, 255);

    if (m_settings.sampleMerged)
    {
        const PixelBuffer& pb = m_editor->compositePixels();
        if (!pb.inBounds(x, y)) return;
        c = pb.getPixel(x, y);
    }
    else
    {
        if (!frame) return;
        PixelBuffer& pb = frame->pixels();
        if (!pb.inBounds(x, y)) return;
        c = pb.getPixel(x, y);
    }

    m_last = c;
    m_editor->setPrimaryColor(c);
}