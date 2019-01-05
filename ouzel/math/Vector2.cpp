// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cassert>
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    template<class T>
    Vector2<T>::Vector2(const Vector3<T>& vec):
        v{vec.v[0], vec.v[1]}
    {
    }

    template<class T>
    Vector2<T>& Vector2<T>::operator=(const Vector3<T>& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        return *this;
    }

    template<class T>
    Vector2<T>::Vector2(const Vector4<T>& vec):
        v{vec.v[0], vec.v[1]}
    {
    }

    template<class T>
    Vector2<T>& Vector2<T>::operator=(const Vector4<T>& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        return *this;
    }

    template<class T>
    void Vector2<T>::clamp(const Vector2& min, const Vector2& max)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1]));

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
    }

    template<class T>
    void Vector2<T>::clamp(const Vector2& vec, const Vector2& min,
                           const Vector2& max, Vector2& dst)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1]));

        // clamp the v[0] value
        dst.v[0] = vec.v[0];
        if (dst.v[0] < min.v[0])
            dst.v[0] = min.v[0];
        if (dst.v[0] > max.v[0])
            dst.v[0] = max.v[0];

        // clamp the v[1] value
        dst.v[1] = vec.v[1];
        if (dst.v[1] < min.v[1])
            dst.v[1] = min.v[1];
        if (dst.v[1] > max.v[1])
            dst.v[1] = max.v[1];
    }

    template<class T>
    void Vector2<T>::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1];
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < std::numeric_limits<float>::min()) // too close to zero
            return;

        n = 1.0F / n;
        v[0] *= n;
        v[1] *= n;
    }

    template<class T>
    void Vector2<T>::rotate(float angle)
    {
        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        float tempX = v[0] * cosAngle - v[1] * sinAngle;
        v[1] = v[1] * cosAngle + v[0] * sinAngle;
        v[0] = tempX;
    }

    template<class T>
    void Vector2<T>::rotate(const Vector2& point, float angle)
    {
        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        if (point.isZero())
        {
            float tempX = v[0] * cosAngle - v[1] * sinAngle;
            v[1] = v[1] * cosAngle + v[0] * sinAngle;
            v[0] = tempX;
        }
        else
        {
            float tempX = v[0] - point.v[0];
            float tempY = v[1] - point.v[1];

            v[0] = tempX * cosAngle - tempY * sinAngle + point.v[0];
            v[1] = tempY * cosAngle + tempX * sinAngle + point.v[1];
        }
    }

    template<class T>
    float Vector2<T>::getMin() const
    {
        return std::min(v[0], v[1]);
    }

    template<class T>
    float Vector2<T>::getMax() const
    {
        return std::max(v[0], v[1]);
    }

    template class Vector2<float>;
}
