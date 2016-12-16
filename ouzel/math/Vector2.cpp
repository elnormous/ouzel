// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "MathUtils.h"

namespace ouzel
{
    const Vector2 Vector2::ZERO(0.0f, 0.0f);
    const Vector2 Vector2::ONE(1.0f, 1.0f);
    const Vector2 Vector2::UNIT_X(1.0f, 0.0f);
    const Vector2 Vector2::UNIT_Y(0.0f, 1.0f);

    Vector2::Vector2(const Vector2& p1, const Vector2& p2)
    {
        set(p1, p2);
    }

    Vector2::Vector2(const Vector3& vec):
        v{ vec.v[0], vec.v[1] }
    {
    }

    Vector2& Vector2::operator=(const Vector3& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        return *this;
    }

    Vector2::Vector2(const Vector4& vec):
        v{ vec.v[0], vec.v[1] }
    {
    }

    Vector2& Vector2::operator=(const Vector4& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        return *this;
    }

    float Vector2::angle(const Vector2& v1, const Vector2& v2)
    {
        float dz = v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0];
        return atan2f(fabsf(dz) + FLOAT_SMALL, dot(v1, v2));
    }

    void Vector2::add(const Vector2& v1, const Vector2& v2, Vector2& dst)
    {
        dst.v[0] = v1.v[0] + v2.v[0];
        dst.v[1] = v1.v[1] + v2.v[1];
    }

    void Vector2::clamp(const Vector2& min, const Vector2& max)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1]));

        // Clamp the x value.
        if (v[0] < min.v[0])
            v[0] = min.v[0];
        if (v[0] > max.v[0])
            v[0] = max.v[0];

        // Clamp the y value.
        if (v[1] < min.v[1])
            v[1] = min.v[1];
        if (v[1] > max.v[1])
            v[1] = max.v[1];
    }

    void Vector2::clamp(const Vector2& vec, const Vector2& min, const Vector2& max, Vector2& dst)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1]));

        // Clamp the x value.
        dst.v[0] = vec.v[0];
        if (dst.v[0] < min.v[0])
            dst.v[0] = min.v[0];
        if (dst.v[0] > max.v[0])
            dst.v[0] = max.v[0];

        // Clamp the y value.
        dst.v[1] = vec.v[1];
        if (dst.v[1] < min.v[1])
            dst.v[1] = min.v[1];
        if (dst.v[1] > max.v[1])
            dst.v[1] = max.v[1];
    }

    float Vector2::distance(const Vector2& vec) const
    {
        float dx = vec.v[0] - v[0];
        float dy = vec.v[1] - v[1];

        return sqrtf(dx * dx + dy * dy);
    }

    float Vector2::length() const
    {
        return sqrtf(v[0] * v[0] + v[1] * v[1]);
    }

    Vector2& Vector2::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1];
        // Already normalized.
        if (n == 1.0f)
            return *this;

        n = sqrtf(n);
        // Too close to zero.
        if (n < TOLERANCE)
            return *this;

        n = 1.0f / n;
        v[0] *= n;
        v[1] *= n;

        return *this;
    }

    void Vector2::rotate(float angle)
    {
        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        float tempX = v[0] * cosAngle - v[1] * sinAngle;
        v[1] = v[1] * cosAngle + v[0] * sinAngle;
        v[0] = tempX;
    }

    void Vector2::rotate(const Vector2& point, float angle)
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

    void Vector2::smooth(const Vector2& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
        {
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }
    }

    float Vector2::getMin() const
    {
        return std::min(v[0], v[1]);
    }

    float Vector2::getMax() const
    {
        return std::max(v[0], v[1]);
    }
}
