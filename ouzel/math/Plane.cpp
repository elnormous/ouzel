// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Plane.hpp"
#include "Vector4.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    template<class T>
    float Plane<T>::dot(const Vector4<T>& vec) const
    {
        return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3];
    }

    template<class T>
    void Plane<T>::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < std::numeric_limits<float>::min()) // too close to zero
            return;

        n = 1.0F / n;
        v[0] *= n;
        v[1] *= n;
        v[2] *= n;
        v[3] *= n;
    }

    template class Plane<float>;
}
