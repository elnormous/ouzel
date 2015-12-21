// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>

namespace ouzel
{
    template <typename T> int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    bool checkOpenGLErrors();
    void log(const char* format, ...);
    long getCurrentMicroSeconds();
}
