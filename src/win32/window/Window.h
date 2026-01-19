#pragma once
#include <Windows.h>

class Window
{
public:
    bool Create(HINSTANCE hInstance, int width, int height);
    HWND GetHWND() const;

private:
    HWND hwnd = nullptr;  // 👈 THIS WAS MISSING
};
