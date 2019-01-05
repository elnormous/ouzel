// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cassert>
#include "Vector4.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Color.hpp"

namespace ouzel
{
    template<class T>
    Vector4<T>::Vector4(Color color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
        v[3] = color.normA();
    }

    template<class T>
    Vector4<T>& Vector4<T>::operator=(Color color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
        v[3] = color.normA();
        return *this;
    }

    template<class T>
    void Vector4<T>::clamp(const Vector4& min, const Vector4& max)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2] || min.v[3] > max.v[3]));

        // clamp the v[0] value
        if (v[0] < min.v[0])
            v[0] = min.v[0];
        if (v[0] > max.v[0])
            v[0] = max.v[0];

        // clamp the v[1] value
        if (v[1] < min.v[1])
            v[1] = min.v[1];
        if (v[1] > max.v[1])
            v[1] = max.v[1];

        // clamp the v[2] value
        if (v[2] < min.v[2])
            v[2] = min.v[2];
        if (v[2] > max.v[2])
            v[2] = max.v[2];

        // clamp the v[2] value
        if (v[3] < min.v[3])
            v[3] = min.v[3];
        if (v[3] > max.v[3])
            v[3] = max.v[3];
    }

    template<class T>
    void Vector4<T>::normalize()
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

    template<class T>
    T Vector4<T>::getMin() const
    {
        return std::min(v[0], std::min(v[1], std::min(v[2], v[3])));
    }

    template<class T>
    T Vector4<T>::getMax() const
    {
        return std::max(v[0], std::max(v[1], std::max(v[2], v[3])));
    }

    template class Vector4<float>;
}
