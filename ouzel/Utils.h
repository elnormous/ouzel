//
//  Utils.h
//  Ouzel
//
//  Created by Elviss Strazdins on 14/03/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

#pragma once

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

bool checkOpenGLErrors();
std::string getResourcePath(const std::string& filename);
void log(const char* format, ...);
