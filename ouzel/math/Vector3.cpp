// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Color.h"
#include "MathUtils.h"

namespace ouzel
{
    const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
    const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);
    const Vector3 Vector3::UNIT_X(1.0f, 0.0f, 0.0f);
    const Vector3 Vector3::UNIT_Y(0.0f, 1.0f, 0.0f);
    const Vector3 Vector3::UNIT_Z(0.0f, 0.0f, 1.0f);

    Vector3::Vector3(const Vector3& p1, const Vector3& p2)
    {
        set(p1, p2);
    }

    Vector3::Vector3(const Vector2& v):
        x(v.x), y(v.y), z(0.0f)
    {
    }

    Vector3::Vector3(const Vector2& v, float aZ):
    x(v.x), y(v.y), z(aZ)
    {
    }

    Vector3& Vector3::operator=(const Vector2& v)
    {
        x = v.x;
        y = v.y;
        z = 0.0f;

        return *this;
    }

    Vector3::Vector3(const Vector4& v)
    {
        set(v.x, v.y, v.z);
    }

    Vector3& Vector3::operator=(const Vector4& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;

        return *this;
    }

    Vector3 Vector3::fromColor(const Color& color)
    {
        return Vector3(static_cast<float>(color.r) / 255.0f,
                       static_cast<float>(color.g) / 255.0f,
                       static_cast<float>(color.b) / 255.0f);
    }

    float Vector3::angle(const Vector3& v1, const Vector3& v2)
    {
        float dx = v1.y * v2.z - v1.z * v2.y;
        float dy = v1.z * v2.x - v1.x * v2.z;
        float dz = v1.x * v2.y - v1.y * v2.x;

        return atan2f(sqrtf(dx * dx + dy * dy + dz * dz) + FLOAT_SMALL, dot(v1, v2));
    }

    void Vector3::clamp(const Vector3& min, const Vector3& max)
    {
        assert(!(min.x > max.x || min.y > max.y || min.z > max.z));

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

        // Clamp the z value.
        if (z < min.z)
            z = min.z;
        if (z > max.z)
            z = max.z;
    }

    void Vector3::clamp(const Vector3& v, const Vector3& min, const Vector3& max, Vector3& dst)
    {
        assert(!(min.x > max.x || min.y > max.y || min.z > max.z));

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

        // Clamp the z value.
        dst.z = v.z;
        if (dst.z < min.z)
            dst.z = min.z;
        if (dst.z > max.z)
            dst.z = max.z;
    }

    void Vector3::cross(const Vector3& v)
    {
        cross(*this, v, *this);
    }

    void Vector3::cross(const Vector3& v1, const Vector3& v2, Vector3& dst)
    {
        dst.x = (v1.y * v2.z) - (v1.z * v2.y);
        dst.y = (v1.z * v2.x) - (v1.x * v2.z);
        dst.z = (v1.x * v2.y) - (v1.y * v2.x);
    }

    float Vector3::distance(const Vector3& v) const
    {
        float dx = v.x - x;
        float dy = v.y - y;
        float dz = v.z - z;

        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    float Vector3::length() const
    {
        return sqrtf(x * x + y * y + z * z);
    }

    Vector3& Vector3::normalize()
    {
        float n = x * x + y * y + z * z;
        // Already normalized.
        if (n == 1.0f)
            return *this;

        n = sqrtf(n);
        // Too close to zero.
        if (n < TOLERANCE)
            return *this;

        n = 1.0f / n;
        x *= n;
        y *= n;
        z *= n;

        return *this;
    }

    void Vector3::smooth(const Vector3& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
        {
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }
    }

    float Vector3::getMin() const
    {
        return std::min(x, std::min(y, z));
    }

    float Vector3::getMax() const
    {
        return std::max(x, std::max(y, z));
    }
}
