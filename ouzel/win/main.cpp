// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#include <windows.h>
#include "WindowWin.h"
#include "core/Engine.h"
#include "utils/Utils.h"

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

    std::shared_ptr<ouzel::WindowWin> window = std::static_pointer_cast<ouzel::WindowWin>(ouzel::sharedEngine->getWindow());
    ouzel::sharedEngine->begin();

    MSG msg;

    bool running = true;

    while (running)
    {
        if (!ouzel::sharedEngine->run())
        {
            running = false;
        }

        std::set<HACCEL> accelerators = window->getAccelerators();

        for (;;)
        {
            if (ouzel::sharedEngine->isRunning())
            {
                if (!PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                {
                    break;
                }
            }
            else
            {
                if (GetMessage(&msg, nullptr, 0, 0) <= 0)
                {
                    running = false;
                    break;
                }
            }

            bool translate = true;

            for (HACCEL accelerator : accelerators)
            {
                if (TranslateAccelerator(window->getNativeWindow(), accelerator, &msg))
                {
                    translate = false;
                }
            }

            if (translate)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (msg.message == WM_QUIT)
            {
                running = false;
            }
        }
    }

    ouzel::sharedEngine->end();

    return 0;
}
