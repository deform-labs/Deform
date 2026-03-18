#include <imgui/imgui.h>
#include <cstdio>
#pragma once

class Config
{
public:
    float clearColor[4] = { 0.05f, 0.5f, 1.0f, 1.0f };
private:
};

extern Config config;   // declaration
