#include <imgui/imgui.h>
#include <cstdio>
#pragma once

// Color4 structure to hold RGBA values
struct Color4
{
    float r, g, b, a;

    Color4() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    // Convert to ImVec4 for ImGui
    ImVec4 toImVec4() const
    {
        return ImVec4(r, g, b, a);
    }

    // Create from ImVec4
    static Color4 fromImVec4(const ImVec4 &v)
    {
        return Color4(v.x, v.y, v.z, v.w);
    }
};

class config
{
public:
    // TODO: make global config header
    void updateHexInput(char hexInput[16], Color4 color)
    {
        snprintf(hexInput, sizeof(hexInput), "%02X%02X%02X%02X",
                 (int)(color.r * 255), (int)(color.g * 255),
                 (int)(color.b * 255), (int)(color.a * 255));
    }
private:
};
