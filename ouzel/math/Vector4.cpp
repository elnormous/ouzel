// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cassert>
#include "Vector4.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Color.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    Vector4::Vector4(Color color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
        v[3] = color.normA();
    }

    Vector4& Vector4::operator=(Color color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
        v[3] = color.normA();
        return *this;
    }

    void Vector4::clamp(const Vector4& min, const Vector4& max)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2] || min.v[3] > max.v[3]));

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

        // clamp the v[2] value
        if (v[3] < min.v[3])
            v[3] = min.v[3];
        if (v[3] > max.v[3])
            v[3] = max.v[3];
    }

    void Vector4::clamp(const Vector4& vec, const Vector4& min, const Vector4& max, Vector4& dst)
    {
        assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2] || min.v[3] > max.v[3]));

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

        // clamp the v[3] value
        dst.v[3] = vec.v[3];
        if (dst.v[3] < min.v[3])
            dst.v[3] = min.v[3];
        if (dst.v[3] > max.v[3])
            dst.v[3] = max.v[3];
    }

    void Vector4::normalize()
    {
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < std::numeric_limits<float>::min()) // too close to zero
            return;

        n = 1.0F / n;
        v[0] *= n;
        v[1] *= n;
        v[2] *= n;
        v[3] *= n;
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
