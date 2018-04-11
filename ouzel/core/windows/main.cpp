// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Windows.h>
#include "EngineWin.hpp"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    std::unique_ptr<ouzel::EngineWin> engine(new ouzel::EngineWin());

    int result = engine->run();
    engine.reset(); // must release engine instance before exit on Windows

    return result;
}
