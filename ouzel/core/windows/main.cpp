// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include "EngineWin.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ouzel::EngineWin engine;

    return engine.run();
}
