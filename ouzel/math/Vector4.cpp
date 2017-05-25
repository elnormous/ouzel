// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include "MathUtils.h"

namespace ouzel
{
    const Vector4 Vector4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);
    const Vector4 Vector4::UNIT_X(1.0f, 0.0f, 0.0f, 1.0f);
    const Vector4 Vector4::UNIT_Y(0.0f, 1.0f, 0.0f, 1.0f);
    const Vector4 Vector4::UNIT_Z(0.0f, 0.0f, 1.0f, 1.0f);
    const Vector4 Vector4::NEGATIVE_UNIT_X(-1.0f, 0.0f, 0.0f, 1.0f);
    const Vector4 Vector4::NEGATIVE_UNIT_Y(0.0f, -1.0f, 0.0f, 1.0f);
    const Vector4 Vector4::NEGATIVE_UNIT_Z(0.0f, 0.0f, -1.0f, 1.0f);

    Vector4::Vector4(const Vector4& p1, const Vector4& p2)
    {
        set(p1, p2);
    }

    Vector4::Vector4(const Vector2& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = 0.0f;
        v[3] = 0.0f;
    }

    Vector4& Vector4::operator=(const Vector2& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = 0.0f;
        v[3] = 0.0f;
        return *this;
    }

    Vector4::Vector4(const Vector3& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = vec.v[2];
        v[3] = 0.0f;
    }

    Vector4& Vector4::operator=(const Vector3& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = vec.v[2];
        v[3] = 0.0f;
        return *this;
    }

    Vector4::Vector4(const Color& color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
        v[3] = color.normA();
    }

    Vector4& Vector4::operator=(const Color& color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
        v[3] = color.normA();
        return *this;
    }

    float Vector4::angle(const Vector4& v1, const Vector4& v2)
    {
        float dx = v1.v[3] * v2.v[0] - v1.v[0] * v2.v[3] - v1.v[1] * v2.v[2] + v1.v[2] * v2.v[1];
        float dy = v1.v[3] * v2.v[1] - v1.v[1] * v2.v[3] - v1.v[2] * v2.v[0] + v1.v[0] * v2.v[2];
        float dz = v1.v[3] * v2.v[2] - v1.v[2] * v2.v[3] - v1.v[0] * v2.v[1] + v1.v[1] * v2.v[0];

        return atan2f(sqrtf(dx * dx + dy * dy + dz * dz) + FLOAT_SMALL, dot(v1, v2));
    }

    void Vector4::clamp(const Vector4& min, const Vector4& max)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2] || min.v[3] > max.v[3]));

        // clamp the x value
        if (v[0] < min.v[0])
            v[0] = min.v[0];
        if (v[0] > max.v[0])
            v[0] = max.v[0];

        // clamp the y value
        if (v[1] < min.v[1])
            v[1] = min.v[1];
        if (v[1] > max.v[1])
            v[1] = max.v[1];

        // clamp the z value
        if (v[2] < min.v[2])
            v[2] = min.v[2];
        if (v[2] > max.v[2])
            v[2] = max.v[2];

        // clamp the z value
        if (v[3] < min.v[3])
            v[3] = min.v[3];
        if (v[3] > max.v[3])
            v[3] = max.v[3];
    }

    void Vector4::clamp(const Vector4& vec, const Vector4& min, const Vector4& max, Vector4& dst)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2] || min.v[3] > max.v[3]));

        // clamp the x value
        dst.v[0] = vec.v[0];
        if (dst.v[0] < min.v[0])
            dst.v[0] = min.v[0];
        if (dst.v[0] > max.v[0])
            dst.v[0] = max.v[0];

        // clamp the y value
        dst.v[1] = vec.v[1];
        if (dst.v[1] < min.v[1])
            dst.v[1] = min.v[1];
        if (dst.v[1] > max.v[1])
            dst.v[1] = max.v[1];

        // clamp the z value
        dst.v[2] = vec.v[2];
        if (dst.v[2] < min.v[2])
            dst.v[2] = min.v[2];
        if (dst.v[2] > max.v[2])
            dst.v[2] = max.v[2];

        // clamp the w value
        dst.v[3] = vec.v[3];
        if (dst.v[3] < min.v[3])
            dst.v[3] = min.v[3];
        if (dst.v[3] > max.v[3])
            dst.v[3] = max.v[3];
    }

    float Vector4::distance(const Vector4& vec) const
    {
        float dx = vec.v[0] - v[0];
        float dy = vec.v[1] - v[1];
        float dz = vec.v[2] - v[2];
        float dw = vec.v[3] - v[3];

        return sqrtf(dx * dx + dy * dy + dz * dz + dw * dw);
    }

    float Vector4::length() const
    {
        return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
    }

    Vector4& Vector4::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        if (n == 1.0f) // already normalized
            return *this;

        n = sqrtf(n);
        if (n < TOLERANCE) // too close to zero
            return *this;

        n = 1.0f / n;
        v[0] *= n;
        v[1] *= n;
        v[2] *= n;
        v[3] *= n;

        return *this;
    }

    void Vector4::smooth(const Vector4& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
        {
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }
    }

    float Vector4::getMin() const
    {
        return std::min(v[0], std::min(v[1], std::min(v[2], v[3])));
    }

    float Vector4::getMax() const
    {
        return std::max(v[0], std::max(v[1], std::max(v[2], v[3])));
    }
}
