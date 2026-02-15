#pragma once
#include "SelectionMask.h"
#include "../model/Frame.h"
#include "../model/PixelBuffer.h"
#include "UndoStack.h"

enum class MoveMode { Cut, Copy };

// Команда перемещения выделения (хранит diff пикселей)
class MoveSelectionCommand : public Command {
public:
    MoveSelectionCommand(Frame* frame, std::vector<PixelDiff> diffs)
        : m_frame(frame), m_diffs(std::move(diffs)) {
    }

    void undo() override {
        PixelBuffer& pb = m_frame->pixels();
        for (auto& d : m_diffs) pb.setPixel(d.x, d.y, d.before);
    }
    void redo() override {
        PixelBuffer& pb = m_frame->pixels();
        for (auto& d : m_diffs) pb.setPixel(d.x, d.y, d.after);
    }

private:
    Frame* m_frame = nullptr;
    std::vector<PixelDiff> m_diffs;
};

class SelectionMoveSession {
public:
    SelectionMoveSession() { }
    bool active() const { return m_active; }

    // Поднять выделение (cut)
    bool begin(Frame* frame, const SelectionMask& sel, MoveMode mode) {
        if (!frame || sel.empty()) return false;

        m_frame = frame;
        m_sel = sel; // копия маски
        m_offsetX = 0; m_offsetY = 0;
        m_mode = mode;

        int bx, by, bw, bh;
        if (!m_sel.bounds(bx, by, bw, bh)) return false;

        m_boundsX = bx; m_boundsY = by;
        m_boundsW = bw; m_boundsH = bh;

        m_floating = PixelBuffer(bw, bh, frame->pixels().format());
        m_floating.clear(PixelRGBA8(0, 0, 0, 0));

        m_cutDiffs.clear();
        if (m_mode == MoveMode::Cut)
            m_cutDiffs.reserve(size_t(bw) * size_t(bh));

        // вырезаем пиксели из frame в floating, на месте делаем прозрачное
        PixelBuffer& pb = m_frame->pixels();
        for (int y = 0; y < bh; ++y) {
            for (int x = 0; x < bw; ++x) {
                int fx = bx + x;
                int fy = by + y;
                if (!m_sel.get(fx, fy)) continue;

                PixelRGBA8 p = pb.getPixel(fx, fy);
                m_floating.setPixel(x, y, p);

                if (m_mode == MoveMode::Cut)
                {
                    PixelRGBA8 before = pb.getPixel(fx, fy);
                    PixelRGBA8 after = PixelRGBA8(0, 0, 0, 0);

                    if (!(before.r == after.r && before.g == after.g &&
                        before.b == after.b && before.a == after.a))
                    {
                        m_cutDiffs.push_back({ fx, fy, before, after });
                    }

                    pb.setPixel(fx, fy, after);
                }

            }
        }

        m_active = true;
        return true;
    }

    void setOffset(int dx, int dy) {
        if (!m_active) return;
        m_offsetX = dx;
        m_offsetY = dy;
    }

    // Для предпросмотра рендера: отрисовать floating поверх кадра
    void renderPreviewTo(PixelBuffer& dst) const {
        if (!m_active) return;
        // dst предполагается уже как базовый frame (после cut)
        dst.blitFrom(
            m_floating,
            0, 0,
            m_floating.width(), m_floating.height(),
            m_boundsX + m_offsetX,
            m_boundsY + m_offsetY
        );
    }

    // Коммитим: записываем floating на новое место и возвращаем Command для undo
    std::unique_ptr<Command> commit() {
        std::vector<PixelDiff> diffs;
        diffs.reserve(m_cutDiffs.size() + size_t(m_boundsW) * size_t(m_boundsH));

        // 1) diffs вырезания (важно для undo)
        for (const auto& d : m_cutDiffs)
            diffs.push_back(d);

        if (!m_active) return nullptr;

        PixelBuffer& pb = m_frame->pixels();

        int dstX0 = m_boundsX + m_offsetX;
        int dstY0 = m_boundsY + m_offsetY;

        for (int y = 0; y < m_boundsH; ++y) {
            for (int x = 0; x < m_boundsW; ++x) {
                PixelRGBA8 srcP = m_floating.getPixel(x, y);
                if (srcP.a == 0) continue; // оптимизация

                int tx = dstX0 + x;
                int ty = dstY0 + y;
                if (!pb.inBounds(tx, ty)) continue;

                PixelRGBA8 before = pb.getPixel(tx, ty);
                PixelRGBA8 after = srcP; // Normal overwrite для простоты (можно blend)

                if (before.r == after.r && before.g == after.g && before.b == after.b && before.a == after.a)
                    continue;

                diffs.push_back({ tx, ty, before, after });
                pb.setPixel(tx, ty, after);
            }
        }

        m_active = false;
        return std::make_unique<MoveSelectionCommand>(m_frame, std::move(diffs));
    }

    // Отмена: вернуть floating обратно на исходное место
    void cancel() {
        if (!m_active) return;
        PixelBuffer& pb = m_frame->pixels();

        // Если это был Cut — восстанавливаем исходную область
        if (m_mode == MoveMode::Cut) {
            for (const auto& d : m_cutDiffs)
                pb.setPixel(d.x, d.y, d.before);
        }

        m_active = false;
    }

    int boundsX() const { return m_boundsX; }
    int boundsY() const { return m_boundsY; }
    int boundsW() const { return m_boundsW; }
    int boundsH() const { return m_boundsH; }

    int offsetX() const { return m_offsetX; }
    int offsetY() const { return m_offsetY; }

private:
    bool m_active = false;
    Frame* m_frame = nullptr;

    MoveMode m_mode = MoveMode::Cut;

    SelectionMask m_sel;     // копия маски
    PixelBuffer m_floating;  // вырезанный фрагмент
    std::vector<PixelDiff> m_cutDiffs;

    int m_boundsX = 0, m_boundsY = 0, m_boundsW = 0, m_boundsH = 0;
    int m_offsetX = 0, m_offsetY = 0;
};
