// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Plane.hpp"
#include "Vector4.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    float Plane::dot(const Vector4& vec) const
    {
        return a * vec.x + b * vec.y + c * vec.z + d;
    }

    void Plane::normalize()
    {
        float n = a * a + b * b + c * c + d * d;
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < SMALL_NUMBER) // too close to zero
            return;

        n = 1.0F / n;
        a *= n;
        b *= n;
        c *= n;
        d *= n;
    }
}
