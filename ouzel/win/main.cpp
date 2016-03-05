// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    LPWSTR* argList;
    int nArgs;
    int i;

    argList = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    std::vector<std::string> args;

    if (argList)
    {
        for (i = 0; i < nArgs; i++)
        {
            char temporaryCString[256];
            WideCharToMultiByte(CP_UTF8, 0, argList[i], -1, temporaryCString, sizeof(temporaryCString), nullptr, nullptr);

            args.push_back(temporaryCString);
        }

        LocalFree(argList);
    }

    ouzel::Engine::getInstance()->setArgs(args);
    ouzel::Engine::getInstance()->init();
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

        if (!ouzel::Engine::getInstance()->run())
        {
            break;
        }
    }

    ouzel::Engine::getInstance()->end();
    
    return 0;
}
