// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Vector3.hpp"
#include "Vector2.hpp"
#include "Vector4.hpp"
#include "Color.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    Vector3::Vector3(const Vector4& vec):
        v{vec.v[0], vec.v[1], vec.v[2]}
    {
    }

    Vector3& Vector3::operator=(const Vector4& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        v[2] = vec.v[2];

        return *this;
    }

    Vector3::Vector3(Color color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
    }

    Vector3& Vector3::operator=(Color color)
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

        return atan2f(sqrtf(dx * dx + dy * dy + dz * dz), dot(v1, v2));
    }

    void Vector3::clamp(const Vector3& min, const Vector3& max)
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

    void Vector3::clamp(const Vector3& vec, const Vector3& min, const Vector3& max, Vector3& dst)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2]));

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

        // clamp the v[2] value
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

    void Vector3::normalize()
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

    void Vector3::smooth(const Vector3& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
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
