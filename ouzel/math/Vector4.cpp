// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "MathUtils.h"

namespace ouzel
{
    const Vector4 Vector4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);
    const Vector4 Vector4::UNIT_X(1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::UNIT_Y(0.0f, 1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::UNIT_Z(0.0f, 0.0f, 1.0f, 0.0f);
    const Vector4 Vector4::UNIT_W(0.0f, 0.0f, 0.0f, 1.0f);

    Vector4::Vector4(const float* src)
    {
        set(src);
    }

    Vector4::Vector4(const Vector4& p1, const Vector4& p2)
    {
        set(p1, p2);
    }

    Vector4::Vector4(const Vector2& v):
        x(v.x),
        y(v.y),
        z(0.0f),
        w(0.0f)
    {
    }

    Vector4& Vector4::operator=(const Vector2& v)
    {
        x = v.x;
        y = v.y;
        z = 0.0f;
        w = 0.0f;

        return *this;
    }

    Vector4::Vector4(const Vector3& v):
        x(v.x),
        y(v.y),
        z(v.z),
        w(0.0f)
    {
    }

    Vector4& Vector4::operator=(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = 0.0f;

        return *this;
    }

    Vector4 Vector4::fromColor(unsigned int color)
    {
        float components[4];
        int componentIndex = 0;
        for (int i = 3; i >= 0; --i)
        {
            int component = (color >> i*8) & 0x000000ff;

            components[componentIndex++] = static_cast<float>(component) / 255.0f;
        }

        Vector4 value(components);
        return value;
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

        // Clamp the z value.
        if (w < min.w)
            w = min.w;
        if (w > max.w)
            w = max.w;
    }

    void Vector4::clamp(const Vector4& v, const Vector4& min, const Vector4& max, Vector4& dst)
    {
        assert(!(min.x > max.x || min.y > max.y || min.z > max.z || min.w > max.w));

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

        // Clamp the w value.
        dst.w = v.w;
        if (dst.w < min.w)
            dst.w = min.w;
        if (dst.w > max.w)
            dst.w = max.w;
    }

    float Vector4::distance(const Vector4& v) const
    {
        float dx = v.x - x;
        float dy = v.y - y;
        float dz = v.z - z;
        float dw = v.w - w;

        return sqrtf(dx * dx + dy * dy + dz * dz + dw * dw);
    }

    float Vector4::length() const
    {
        return sqrtf(x * x + y * y + z * z + w * w);
    }

    Vector4& Vector4::normalize()
    {
        normalize(*this);
        return *this;
    }

    void Vector4::normalize(Vector4& dst) const
    {
        if (&dst != this)
        {
            dst.x = x;
            dst.y = y;
            dst.z = z;
            dst.w = w;
        }

        float n = x * x + y * y + z * z + w * w;
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
        dst.z *= n;
        dst.w *= n;
    }

    void Vector4::set(const float* array)
    {
        assert(array);

        x = array[0];
        y = array[1];
        z = array[2];
        w = array[3];
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
        return std::min(x, std::min(y, std::min(z, w)));
    }

    float Vector4::getMax() const
    {
        return std::max(x, std::max(y, std::max(z, w)));
    }
}
