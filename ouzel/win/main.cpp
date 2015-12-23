// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include "../Engine.h"

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    ouzel::Engine engine;
    engine.begin();
    
    return 0;
}