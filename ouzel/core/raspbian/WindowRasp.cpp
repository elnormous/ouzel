// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowRasp.h"

namespace ouzel
{
    WindowRasp::WindowRasp(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
    {
    }

    WindowRasp::~WindowRasp()
    {
    }
}
