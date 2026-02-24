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
        // берём то, что видно (композит)
        const PixelBuffer& pb = m_editor->compositePixels();
        if (!pb.inBounds(x, y)) return;
        c = pb.getPixel(x, y);
    }
    else
    {
        // берём только из активного слоя (frame = activeCelFrame)
        if (!frame) return;
        PixelBuffer& pb = frame->pixels();
        if (!pb.inBounds(x, y)) return;
        c = pb.getPixel(x, y);
    }

    m_last = c;

    // сообщаем редактору о новом цвете
    m_editor->setPrimaryColor(c);
}