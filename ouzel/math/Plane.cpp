// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Plane.hpp"
#include "Vector4.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    float Plane::dot(const Vector4& vec) const
    {
        return a * vec.v[0] + b * vec.v[1] + c * vec.v[2] + d;
    }

    void Plane::normalize()
    {
        float n = a * a + b * b + c * c + d * d;
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < std::numeric_limits<float>::min()) // too close to zero
            return;

        n = 1.0F / n;
        a *= n;
        b *= n;
        c *= n;
        d *= n;
    }
}
