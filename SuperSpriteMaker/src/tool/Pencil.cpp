#include "Pencil.h"

PencilTool::PencilTool(const PixelRGBA8& color) : m_color(color) { }

void PencilTool::setColor(const PixelRGBA8& c) { m_color = c; }

void PencilTool::apply(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure)
{
    if (!frame || !cmd) return;

    PixelBuffer& pb = frame->pixels();

    if (!pb.inBounds(x, y)) return;

    PixelRGBA8 before = pb.getPixel(x, y);

    PixelRGBA8 after = m_color;

    if (before.r == after.r && before.g == after.g && before.b == after.b && before.a == after.a) return;

    cmd->recordPixel(x, y, before, after);
    pb.setPixel(x, y, after);
}
