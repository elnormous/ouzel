// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cassert>
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

    void Vector3::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        if (n == 1.0F) // already normalized
            return;

        n = std::sqrtf(n);
        if (n < std::numeric_limits<float>::min()) // too close to zero
            return;

        n = 1.0F / n;
        v[0] *= n;
        v[1] *= n;
        v[2] *= n;
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
