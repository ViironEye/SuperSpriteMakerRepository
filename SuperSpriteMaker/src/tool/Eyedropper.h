#pragma once
#include "Tool.h"
#include "../structs/PixelFormatStruct.h"

struct EyedropperSettings
{
    bool sampleMerged = true;
};

class SpriteEditor;

class EyedropperTool : public Tool
{
public:
    explicit EyedropperTool(SpriteEditor* editor) : m_editor(editor) {}

    void apply(Frame* frame, StrokeCommand* cmd, int x, int y, float pressure) override;

    const PixelRGBA8& pickedColor() const { return m_last; }

    EyedropperSettings& settings() { return m_settings; }
    const EyedropperSettings& settings() const { return m_settings; }
    void setSettings(const EyedropperSettings& s) { m_settings = s; }

private:
    SpriteEditor* m_editor = nullptr;
    PixelRGBA8 m_last{ 0,0,0,255 };
    EyedropperSettings m_settings;
};