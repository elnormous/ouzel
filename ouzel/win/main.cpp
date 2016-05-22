// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#include <windows.h>
#include "Engine.h"
#include "Utils.h"

void ouzelMain(const std::vector<std::string>& args);

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    OUZEL_UNUSED(hInstance);
    OUZEL_UNUSED(hPrevInstance);
    OUZEL_UNUSED(lpCmdLine);
    OUZEL_UNUSED(nCmdShow);

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

    ouzel::setArgs(args);
    ouzelMain(ouzel::getArgs());
    
    if (!ouzel::sharedEngine)
    {
        return 0;
    }
    
    ouzel::sharedEngine->begin();

    MSG msg;

    for (;;)
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

        if (!ouzel::sharedEngine->run())
        {
            break;
        }
    }

    ouzel::sharedEngine->end();

    return 0;
}
