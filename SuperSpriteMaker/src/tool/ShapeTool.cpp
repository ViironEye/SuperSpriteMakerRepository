#include "ShapeTool.h"
#include "../model/Frame.h"
#include "../model/PixelBuffer.h"
#include <cmath>
#include <algorithm>

void ShapeTool::begin(Frame*, StrokeCommand*, int x, int y, float)
{
    m_active = true;
    m_x0 = m_x1 = x;
    m_y0 = m_y1 = y;
}

void ShapeTool::update(Frame*, StrokeCommand*, int x, int y, float)
{
    if (!m_active) return;
    m_x1 = x;
    m_y1 = y;
}

void ShapeTool::end(Frame* frame, StrokeCommand* cmd, int x, int y, float)
{
    if (!m_active) return;

    m_x1 = x;
    m_y1 = y;

    rasterize(frame, cmd);
    m_active = false;
}

void ShapeTool::rasterize(Frame* frame, StrokeCommand* cmd)
{
    switch (m_settings.mode)
    {
    case ShapeMode::Rectangle: drawRect(frame, cmd); break;
    case ShapeMode::Ellipse: drawEllipse(frame, cmd); break;
    case ShapeMode::Line: drawLine(frame, cmd); break;
    }
}

void ShapeTool::drawRect(Frame* frame, StrokeCommand* cmd)
{
    PixelBuffer& pb = frame->pixels();

    int x0 = std::min(m_x0, m_x1);
    int y0 = std::min(m_y0, m_y1);
    int x1 = std::max(m_x0, m_x1);
    int y1 = std::max(m_y0, m_y1);

    if (m_settings.filled)
    {
        for (int y = y0; y <= y1; ++y)
        {
            for (int x = x0; x <= x1; ++x)
            {
                if (!pb.inBounds(x, y)) continue;
                PixelRGBA8 before = pb.getPixel(x, y);
                pb.setPixel(x, y, m_color);
                cmd->recordPixel(x, y, before, m_color);
            }
        }
    }
    else
    {
        int t = std::max(1, m_settings.thickness);

        for (int i = 0; i < t; ++i)
        {
            for (int x = x0; x <= x1; ++x)
            {
                int yTop = y0 + i;
                int yBot = y1 - i;

                if (pb.inBounds(x, yTop))
                {
                    auto before = pb.getPixel(x, yTop);
                    pb.setPixel(x, yTop, m_color);
                    cmd->recordPixel(x, yTop, before, m_color);
                }
                if (pb.inBounds(x, yBot))
                {
                    auto before = pb.getPixel(x, yBot);
                    pb.setPixel(x, yBot, m_color);
                    cmd->recordPixel(x, yBot, before, m_color);
                }
            }

            for (int y = y0; y <= y1; ++y)
            {
                int xLeft = x0 + i;
                int xRight = x1 - i;

                if (pb.inBounds(xLeft, y))
                {
                    auto before = pb.getPixel(xLeft, y);
                    pb.setPixel(xLeft, y, m_color);
                    cmd->recordPixel(xLeft, y, before, m_color);
                }
                if (pb.inBounds(xRight, y))
                {
                    auto before = pb.getPixel(xRight, y);
                    pb.setPixel(xRight, y, m_color);
                    cmd->recordPixel(xRight, y, before, m_color);
                }
            }
        }
    }
}

void ShapeTool::drawLine(Frame* frame, StrokeCommand* cmd)
{
    PixelBuffer& pb = frame->pixels();

    int x0 = m_x0, y0 = m_y0;
    int x1 = m_x1, y1 = m_y1;

    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        if (pb.inBounds(x0, y0))
        {
            auto before = pb.getPixel(x0, y0);
            pb.setPixel(x0, y0, m_color);
            cmd->recordPixel(x0, y0, before, m_color);
        }

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void ShapeTool::drawEllipse(Frame* frame, StrokeCommand* cmd)
{
    PixelBuffer& pb = frame->pixels();

    int x0 = std::min(m_x0, m_x1);
    int y0 = std::min(m_y0, m_y1);
    int x1 = std::max(m_x0, m_x1);
    int y1 = std::max(m_y0, m_y1);

    int cx = (x0 + x1) / 2;
    int cy = (y0 + y1) / 2;

    int a = (x1 - x0) / 2;
    int b = (y1 - y0) / 2;

    if (a <= 0 && b <= 0) return;

    auto plot = [&](int px, int py)
        {
            if (!pb.inBounds(px, py)) return;
            PixelRGBA8 before = pb.getPixel(px, py);
            if (before.r == m_color.r && before.g == m_color.g && before.b == m_color.b && before.a == m_color.a) return;
            pb.setPixel(px, py, m_color);
            cmd->recordPixel(px, py, before, m_color);
        };

    auto hline = [&](int xL, int xR, int y)
        {
            if (y < 0 || y >= pb.height()) return;
            if (xL > xR) std::swap(xL, xR);
            xL = std::max(xL, 0);
            xR = std::min(xR, pb.width() - 1);
            for (int x = xL; x <= xR; ++x) plot(x, y);
        };

    int thickness = std::max(1, m_settings.thickness);

    auto drawOneEllipseOutline = [&](int ra, int rb)
        {
            if (ra <= 0 && rb <= 0)
            {
                plot(cx, cy);
                return;
            }
            if (ra <= 0)
            {
                for (int y = -rb; y <= rb; ++y) plot(cx, cy + y);
                return;
            }
            if (rb <= 0)
            {
                for (int x = -ra; x <= ra; ++x) plot(cx + x, cy);
                return;
            }

            long long a2 = 1LL * ra * ra;
            long long b2 = 1LL * rb * rb;

            long long x = 0;
            long long y = rb;

            long long dx = 2LL * b2 * x;
            long long dy = 2LL * a2 * y;

            long long d1 = b2 - a2 * rb + a2 / 4;

            while (dx < dy)
            {
                plot(cx + (int)x, cy + (int)y);
                plot(cx - (int)x, cy + (int)y);
                plot(cx + (int)x, cy - (int)y);
                plot(cx - (int)x, cy - (int)y);

                if (d1 < 0)
                {
                    x++;
                    dx += 2LL * b2;
                    d1 += dx + b2;
                }
                else
                {
                    x++;
                    y--;
                    dx += 2LL * b2;
                    dy -= 2LL * a2;
                    d1 += dx - dy + b2;
                }
            }

            long long d2 = b2 * (x + 1LL / 2) * (x + 1LL / 2) + a2 * (y - 1) * (y - 1) - a2 * b2;
            long long X2 = 2LL * x + 1;
            long long D2 = b2 * X2 * X2 + 4LL * a2 * (y - 1) * (y - 1) - 4LL * a2 * b2;

            while (y >= 0)
            {
                plot(cx + (int)x, cy + (int)y);
                plot(cx - (int)x, cy + (int)y);
                plot(cx + (int)x, cy - (int)y);
                plot(cx - (int)x, cy - (int)y);

                if (D2 > 0)
                {
                    y--;
                    dy -= 2LL * a2;
                    D2 += 4LL * a2 - dy;
                }
                else
                {
                    y--;
                    x++;
                    dx += 2LL * b2;
                    dy -= 2LL * a2;
                    D2 += dx - dy + 4LL * a2;
                }
            }
        };

    auto drawOneEllipseFilled = [&](int ra, int rb)
        {
            if (ra <= 0 && rb <= 0) { plot(cx, cy); return; }
            if (ra <= 0) { for (int y = -rb; y <= rb; ++y) plot(cx, cy + y); return; }
            if (rb <= 0) { for (int x = -ra; x <= ra; ++x) plot(cx + x, cy); return; }

            long long a2 = 1LL * ra * ra;
            long long b2 = 1LL * rb * rb;

            long long x = 0;
            long long y = rb;

            long long dx = 2LL * b2 * x;
            long long dy = 2LL * a2 * y;

            long long d1 = b2 - a2 * rb + a2 / 4;

            while (dx < dy)
            {
                hline(cx - (int)x, cx + (int)x, cy + (int)y);
                hline(cx - (int)x, cx + (int)x, cy - (int)y);

                if (d1 < 0)
                {
                    x++;
                    dx += 2LL * b2;
                    d1 += dx + b2;
                }
                else
                {
                    x++;
                    y--;
                    dx += 2LL * b2;
                    dy -= 2LL * a2;
                    d1 += dx - dy + b2;
                }
            }

            long long X2 = 2LL * x + 1;
            long long D2 = b2 * X2 * X2 + 4LL * a2 * (y - 1) * (y - 1) - 4LL * a2 * b2;

            while (y >= 0)
            {
                hline(cx - (int)x, cx + (int)x, cy + (int)y);
                hline(cx - (int)x, cx + (int)x, cy - (int)y);

                if (D2 > 0)
                {
                    y--;
                    dy -= 2LL * a2;
                    D2 += 4LL * a2 - dy;
                }
                else
                {
                    y--;
                    x++;
                    dx += 2LL * b2;
                    dy -= 2LL * a2;
                    D2 += dx - dy + 4LL * a2;
                }
            }
        };

    if (m_settings.filled)
    {
        drawOneEllipseFilled(a, b);
    }
    else
    {

        for (int i = 0; i < thickness; ++i)
        {
            int ra = a - i;
            int rb = b - i;
            if (ra < 0 || rb < 0) break;
            drawOneEllipseOutline(ra, rb);
        }
    }
}