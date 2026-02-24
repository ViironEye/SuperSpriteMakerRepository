#pragma once
#include <string>

class SpriteEditor;

namespace ImageImport
{
    bool importAsNewLayer(SpriteEditor& editor, const std::string& path, bool nearest = true);
}