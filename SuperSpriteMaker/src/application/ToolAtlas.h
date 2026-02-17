#pragma once
#include <glad/glad.h>

class ToolAtlas
{
public:
    ToolAtlas() = default;
    ~ToolAtlas();

    ToolAtlas(const ToolAtlas&) = delete;
    ToolAtlas& operator=(const ToolAtlas&) = delete;

    bool create();

    GLuint texture() const { return m_tex; }
    int cell() const { return 32; }
    int cols() const { return 7; }
    int rows() const { return 1; }

    void getUV(int idx, float& u0, float& v0, float& u1, float& v1) const;

private:
    GLuint m_tex = 0;
};
