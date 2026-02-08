#pragma once
#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>

enum class SelectionOp {
    Replace,
    Add,
    Subtract
};

class SelectionMask {
public:
    SelectionMask() = default;
    SelectionMask(int w, int h) { resize(w, h); }

    void resize(int w, int h) {
        m_w = w; m_h = h;
        m_mask.assign(size_t(w) * size_t(h), 0);
        m_has = false;
    }

    int width()  const { return m_w; }
    int height() const { return m_h; }

    bool empty() const { return !m_has; }

    void clear() {
        std::fill(m_mask.begin(), m_mask.end(), 0);
        m_has = false;
    }

    bool inBounds(int x, int y) const {
        return x >= 0 && y >= 0 && x < m_w && y < m_h;
    }

    void set(int x, int y, bool v) {
        if (!inBounds(x, y)) return;
        m_mask[size_t(y) * m_w + x] = v ? 1 : 0;
        if (v) m_has = true;
    }

    bool get(int x, int y) const {
        if (!inBounds(x, y)) return false;
        return m_mask[size_t(y) * m_w + x] != 0;
    }

    // Прямоугольное выделение (replace)
    void setRect(int x, int y, int w, int h) {
        clear();
        for (int yy = 0; yy < h; ++yy)
            for (int xx = 0; xx < w; ++xx)
                set(x + xx, y + yy, true);
    }

    // Bounding box выделения, чтобы move был быстрым
    bool bounds(int& outX, int& outY, int& outW, int& outH) const {
        if (!m_has) return false;
        int minX = m_w, minY = m_h, maxX = -1, maxY = -1;
        for (int y = 0; y < m_h; ++y) {
            for (int x = 0; x < m_w; ++x) {
                if (get(x, y)) {
                    minX = std::min(minX, x);
                    minY = std::min(minY, y);
                    maxX = std::max(maxX, x);
                    maxY = std::max(maxY, y);
                }
            }
        }
        if (maxX < minX || maxY < minY) return false;
        outX = minX; outY = minY;
        outW = maxX - minX + 1;
        outH = maxY - minY + 1;
        return true;
    }
    void applyRect(int x, int y, int w, int h, SelectionOp op)
    {
        if (op == SelectionOp::Replace)
            clear();

        for (int yy = 0; yy < h; ++yy)
            for (int xx = 0; xx < w; ++xx)
            {
                int px = x + xx;
                int py = y + yy;
                if (!inBounds(px, py)) continue;

                if (op == SelectionOp::Add)
                    set(px, py, true);
                else if (op == SelectionOp::Subtract)
                    set(px, py, false);
                else // Replace уже очистил
                    set(px, py, true);
            }

        // пересчитать m_has после Subtract/Replace (простое решение)
        if (op != SelectionOp::Add) {
            m_has = false;
            for (auto v : m_mask) { if (v) { m_has = true; break; } }
        }
    }

    void invert()
    {
        for (auto& v : m_mask) v = v ? 0 : 1;
        // после инверта точно есть выделение, если размер > 0
        m_has = (m_w > 0 && m_h > 0);
    }


private:
    int m_w = 0, m_h = 0;
    bool m_has = false;
    std::vector<uint8_t> m_mask; // 0/1
};
