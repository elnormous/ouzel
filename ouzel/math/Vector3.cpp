// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cassert>
#include "Vector3.hpp"
#include "Vector2.hpp"
#include "Vector4.hpp"
#include "Color.hpp"

namespace ouzel
{
    template<class T>
    Vector3<T>::Vector3(const Vector4<T>& vec):
        v{vec.v[0], vec.v[1], vec.v[2]}
    {
    }

    template<class T>
    Vector3<T>& Vector3<T>::operator=(const Vector4<T>& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = vec.v[2];

        return *this;
    }

    template<class T>
    Vector3<T>::Vector3(Color color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
    }

    template<class T>
    Vector3<T>& Vector3<T>::operator=(Color color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();

        return *this;
    }

    template<class T>
    void Vector3<T>::clamp(const Vector3& min, const Vector3& max)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2]));

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
    }

    template<class T>
    void Vector3<T>::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < std::numeric_limits<float>::min()) // too close to zero
            return;

        n = 1.0F / n;
        v[0] *= n;
        v[1] *= n;
        v[2] *= n;
    }

    template<class T>
    T Vector3<T>::getMin() const
    {
        return std::min(v[0], std::min(v[1], v[2]));
    }

    template<class T>
    T Vector3<T>::getMax() const
    {
        return std::max(v[0], std::max(v[1], v[2]));
    }

    template class Vector3<float>;
}
