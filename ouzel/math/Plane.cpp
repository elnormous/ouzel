// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Plane.h"
#include "Vector4.h"

namespace ouzel
{
    float Plane::dot(const Vector4& vec) const
    {
        return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] - v[3];
    }
}
