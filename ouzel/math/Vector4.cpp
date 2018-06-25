// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Vector4.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Color.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    Vector4::Vector4(const Vector2& vec):
        x(vec.x), y(vec.y), z(0.0F), w(0.0F)
    {
    }

    Vector4& Vector4::operator=(const Vector2& vec)
    {
        x = vec.x;
        y = vec.y;
        z = 0.0F;
        w = 0.0F;
        return *this;
    }

    Vector4::Vector4(const Vector3& vec):
        x(vec.x), y(vec.y), z(vec.z), w(0.0F)
    {
    }

    Vector4& Vector4::operator=(const Vector3& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        w = 0.0F;
        return *this;
    }

    Vector4::Vector4(const Color& color)
    {
        x = color.normR();
        y = color.normG();
        z = color.normB();
        w = color.normA();
    }

    Vector4& Vector4::operator=(const Color& color)
    {
        x = color.normR();
        y = color.normG();
        z = color.normB();
        w = color.normA();
        return *this;
    }

    float Vector4::angle(const Vector4& v1, const Vector4& v2)
    {
        float dx = v1.w * v2.x - v1.x * v2.w - v1.y * v2.z + v1.z * v2.y;
        float dy = v1.w * v2.y - v1.y * v2.w - v1.z * v2.x + v1.x * v2.z;
        float dz = v1.w * v2.z - v1.z * v2.w - v1.x * v2.y + v1.y * v2.x;

        return atan2f(sqrtf(dx * dx + dy * dy + dz * dz) + FLOAT_SMALL, dot(v1, v2));
    }

    void Vector4::clamp(const Vector4& min, const Vector4& max)
    {
        assert(!(min.x > max.x || min.y > max.y || min.z > max.z || min.w > max.w));

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

        // clamp the z value
        if (z < min.z)
            z = min.z;
        if (z > max.z)
            z = max.z;

        // clamp the z value
        if (w < min.w)
            w = min.w;
        if (w > max.w)
            w = max.w;
    }

    void Vector4::clamp(const Vector4& vec, const Vector4& min, const Vector4& max, Vector4& dst)
    {
        assert(!(min.x > max.x || min.y > max.y || min.z > max.z || min.w > max.w));

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

        // clamp the z value
        dst.z = vec.z;
        if (dst.z < min.z)
            dst.z = min.z;
        if (dst.z > max.z)
            dst.z = max.z;

        // clamp the w value
        dst.w = vec.w;
        if (dst.w < min.w)
            dst.w = min.w;
        if (dst.w > max.w)
            dst.w = max.w;
    }

    float Vector4::distance(const Vector4& vec) const
    {
        float dx = vec.x - x;
        float dy = vec.y - y;
        float dz = vec.z - z;
        float dw = vec.w - w;

        return sqrtf(dx * dx + dy * dy + dz * dz + dw * dw);
    }

    float Vector4::length() const
    {
        return sqrtf(x * x + y * y + z * z + w * w);
    }

    void Vector4::normalize()
    {
        float n = x * x + y * y + z * z + w * w;
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < EPSILON) // too close to zero
            return;

        n = 1.0F / n;
        x *= n;
        y *= n;
        z *= n;
        w *= n;
    }

    void Vector4::smooth(const Vector4& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
    }

    float Vector4::getMin() const
    {
        return std::min(x, std::min(y, std::min(z, w)));
    }

    float Vector4::getMax() const
    {
        return std::max(x, std::max(y, std::max(z, w)));
    }
}
