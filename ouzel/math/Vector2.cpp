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

    Vector2::Vector2(const float* array)
    {
        set(array);
    }

    Vector2::Vector2(const Vector2& p1, const Vector2& p2)
    {
        set(p1, p2);
    }

    Vector2::Vector2(const Vector3& v):
        x(v.x),
        y(v.y)
    {
    }

    Vector2& Vector2::operator=(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    Vector2::Vector2(const Vector4& v):
        x(v.x),
        y(v.y)
    {
    }

    Vector2& Vector2::operator=(const Vector4& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    float Vector2::angle(const Vector2& v1, const Vector2& v2)
    {
        float dz = v1.x * v2.y - v1.y * v2.x;
        return atan2f(fabsf(dz) + FLOAT_SMALL, dot(v1, v2));
    }

    void Vector2::add(const Vector2& v1, const Vector2& v2, Vector2& dst)
    {
        dst.x = v1.x + v2.x;
        dst.y = v1.y + v2.y;
    }

    void Vector2::clamp(const Vector2& min, const Vector2& max)
    {
        assert(!(min.x > max.x || min.y > max.y));

        // Clamp the x value.
        if (x < min.x)
            x = min.x;
        if (x > max.x)
            x = max.x;

        // Clamp the y value.
        if (y < min.y)
            y = min.y;
        if (y > max.y)
            y = max.y;
    }

    void Vector2::clamp(const Vector2& v, const Vector2& min, const Vector2& max, Vector2& dst)
    {
        assert(!(min.x > max.x || min.y > max.y));

        // Clamp the x value.
        dst.x = v.x;
        if (dst.x < min.x)
            dst.x = min.x;
        if (dst.x > max.x)
            dst.x = max.x;

        // Clamp the y value.
        dst.y = v.y;
        if (dst.y < min.y)
            dst.y = min.y;
        if (dst.y > max.y)
            dst.y = max.y;
    }

    float Vector2::distance(const Vector2& v) const
    {
        float dx = v.x - x;
        float dy = v.y - y;

        return sqrtf(dx * dx + dy * dy);
    }

    float Vector2::length() const
    {
        return sqrtf(x * x + y * y);
    }

    Vector2& Vector2::normalize()
    {
        normalize(*this);
        return *this;
    }

    void Vector2::normalize(Vector2& dst) const
    {
        if (&dst != this)
        {
            dst.x = x;
            dst.y = y;
        }

        float n = x * x + y * y;
        // Already normalized.
        if (n == 1.0f)
            return;

        n = sqrtf(n);
        // Too close to zero.
        if (n < TOLERANCE)
            return;

        n = 1.0f / n;
        dst.x *= n;
        dst.y *= n;
    }

    void Vector2::rotate(float angle)
    {
        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        float tempX = x * cosAngle + y * sinAngle;
        y = y * cosAngle - x * sinAngle;
        x = tempX;
    }

    void Vector2::rotate(const Vector2& point, float angle)
    {
        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        if (point.isZero())
        {
            float tempX = x * cosAngle + y * sinAngle;
            y = y * cosAngle - x * sinAngle;
            x = tempX;
        }
        else
        {
            float tempX = x - point.x;
            float tempY = y - point.y;

            x = tempX * cosAngle + tempY * sinAngle + point.x;
            y = tempY * cosAngle - tempX * sinAngle + point.y;
        }
    }

    void Vector2::set(const float* array)
    {
        assert(array);

        x = array[0];
        y = array[1];
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
        return std::min(x, y);
    }

    float Vector2::getMax() const
    {
        return std::max(x, y);
    }
}
