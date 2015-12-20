// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

bool checkOpenGLErrors();
std::string getResourcePath(const std::string& filename);
void log(const char* format, ...);
