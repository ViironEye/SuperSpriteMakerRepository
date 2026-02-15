#include "../enums/ShapeMode.h"
struct ShapeSettings
{
    ShapeMode mode = ShapeMode::Rectangle;
    bool filled = false;
    int thickness = 1;     // для outline (в canvas пикселях)
};
