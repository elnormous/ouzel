// Copyright (C) 2017 Elviss Strazdins
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

    Vector3::Vector3(const Vector2& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = 0.0f;
    }

    Vector3::Vector3(const Vector2& vec, float aZ)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = aZ;
    }

    Vector3& Vector3::operator=(const Vector2& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = 0.0f;

        return *this;
    }

    Vector3::Vector3(const Vector4& vec)
    {
        set(vec.v[0], vec.v[1], vec.v[2]);
    }

    Vector3& Vector3::operator=(const Vector4& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = vec.v[2];

        return *this;
    }

    Vector3::Vector3(const Color& color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
    }

    Vector3& Vector3::operator=(const Color& color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();

        return *this;
    }

    float Vector3::angle(const Vector3& v1, const Vector3& v2)
    {
        float dx = v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1];
        float dy = v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2];
        float dz = v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0];

        return atan2f(sqrtf(dx * dx + dy * dy + dz * dz) + FLOAT_SMALL, dot(v1, v2));
    }

    void Vector3::clamp(const Vector3& min, const Vector3& max)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2]));

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

        // Clamp the z value.
        if (v[2] < min.v[2])
            v[2] = min.v[2];
        if (v[2] > max.v[2])
            v[2] = max.v[2];
    }

    void Vector3::clamp(const Vector3& vec, const Vector3& min, const Vector3& max, Vector3& dst)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2]));

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

        // Clamp the z value.
        dst.v[2] = vec.v[2];
        if (dst.v[2] < min.v[2])
            dst.v[2] = min.v[2];
        if (dst.v[2] > max.v[2])
            dst.v[2] = max.v[2];
    }

    float Vector3::distance(const Vector3& vec) const
    {
        float dx = vec.v[0] - v[0];
        float dy = vec.v[1] - v[1];
        float dz = vec.v[2] - v[2];

        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    float Vector3::length() const
    {
        return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }

    Vector3& Vector3::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
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
        v[2] *= n;

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
        return std::min(v[0], std::min(v[1], v[2]));
    }

    float Vector3::getMax() const
    {
        return std::max(v[0], std::max(v[1], v[2]));
    }
}
