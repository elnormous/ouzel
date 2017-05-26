// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Plane.h"
#include "Vector4.h"
#include "MathUtils.h"

namespace ouzel
{
    float Plane::dot(const Vector4& vec) const
    {
        return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3];
    }

    void Plane::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        if (n == 1.0f) // already normalized
            return;

        n = sqrtf(n);
        if (n < TOLERANCE) // too close to zero
            return;

        n = 1.0f / n;
        v[0] *= n;
        v[1] *= n;
        v[2] *= n;
        v[3] *= n;
    }
}
