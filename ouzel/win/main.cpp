// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    ouzel::Engine::getInstance()->begin();

    MSG msg;

    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            break;
        }

        ouzel::Engine::getInstance()->run();
    }
    
    return 0;
}
