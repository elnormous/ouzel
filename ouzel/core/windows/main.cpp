// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#include <windows.h>
#include <vector>
#include <string>
#include "ApplicationWin.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
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

    ouzel::ApplicationWin application(args);

    return application.run();
}
