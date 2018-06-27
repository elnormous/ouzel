// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    Vector2::Vector2(const Vector3& vec):
        x(vec.x), y(vec.y)
    {
    }

    Vector2& Vector2::operator=(const Vector3& vec)
    {
        x = vec.x;
        y = vec.y;
        return *this;
    }

    Vector2::Vector2(const Vector4& vec):
        x(vec.x), y(vec.y)
    {
    }

    Vector2& Vector2::operator=(const Vector4& vec)
    {
        x = vec.x;
        y = vec.y;
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

        // clamp the x value
        if (x < min.x)
            x = min.x;
        if (x > max.x)
            x = max.x;

        // clamp the y value
        if (y < min.y)
            y = min.y;
        if (y > max.y)
            y = max.y;
    }

    void Vector2::clamp(const Vector2& vec, const Vector2& min, const Vector2& max, Vector2& dst)
    {
        assert(!(min.x > max.x || min.y > max.y));

        // clamp the x value
        dst.x = vec.x;
        if (dst.x < min.x)
            dst.x = min.x;
        if (dst.x > max.x)
            dst.x = max.x;

        // clamp the y value
        dst.y = vec.y;
        if (dst.y < min.y)
            dst.y = min.y;
        if (dst.y > max.y)
            dst.y = max.y;
    }

    float Vector2::distance(const Vector2& vec) const
    {
        float dx = vec.x - x;
        float dy = vec.y - y;

        return sqrtf(dx * dx + dy * dy);
    }

    float Vector2::length() const
    {
        return sqrtf(x * x + y * y);
    }

    void Vector2::normalize()
    {
        float n = x * x + y * y;
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < EPSILON) // too close to zero
            return;

        n = 1.0F / n;
        x *= n;
        y *= n;
    }

    void Vector2::rotate(float angle)
    {
        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        float tempX = x * cosAngle - y * sinAngle;
        y = y * cosAngle + x * sinAngle;
        x = tempX;
    }

    void Vector2::rotate(const Vector2& point, float angle)
    {
        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        if (point.isZero())
        {
            float tempX = x * cosAngle - y * sinAngle;
            y = y * cosAngle + x * sinAngle;
            x = tempX;
        }
        else
        {
            float tempX = x - point.x;
            float tempY = y - point.y;

            x = tempX * cosAngle - tempY * sinAngle + point.x;
            y = tempY * cosAngle + tempX * sinAngle + point.y;
        }
    }

    void Vector2::smooth(const Vector2& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
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
