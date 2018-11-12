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
        x(vec.x), y(vec.y), z(vec.z)
    {
    }

    Vector3& Vector3::operator=(const Vector4& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;

        return *this;
    }

    Vector3::Vector3(Color color)
    {
        x = color.normR();
        y = color.normG();
        z = color.normB();
    }

    Vector3& Vector3::operator=(Color color)
    {
        x = color.normR();
        y = color.normG();
        z = color.normB();

        return *this;
    }

    float Vector3::angle(const Vector3& v1, const Vector3& v2)
    {
        float dx = v1.y * v2.z - v1.z * v2.y;
        float dy = v1.z * v2.x - v1.x * v2.z;
        float dz = v1.x * v2.y - v1.y * v2.x;

        return atan2f(sqrtf(dx * dx + dy * dy + dz * dz), dot(v1, v2));
    }

    void Vector3::clamp(const Vector3& min, const Vector3& max)
    {
        assert(!(min.x > max.x || min.y > max.y || min.z > max.z));

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
    }

    void Vector3::clamp(const Vector3& vec, const Vector3& min, const Vector3& max, Vector3& dst)
    {
        assert(!(min.x > max.x || min.y > max.y || min.z > max.z));

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
    }

    float Vector3::distance(const Vector3& vec) const
    {
        float dx = vec.x - x;
        float dy = vec.y - y;
        float dz = vec.z - z;

        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    float Vector3::length() const
    {
        return sqrtf(x * x + y * y + z * z);
    }

    void Vector3::normalize()
    {
        float n = x * x + y * y + z * z;
        if (n == 1.0F) // already normalized
            return;

        n = sqrtf(n);
        if (n < std::numeric_limits<float>::min()) // too close to zero
            return;

        n = 1.0F / n;
        x *= n;
        y *= n;
        z *= n;
    }

    void Vector3::smooth(const Vector3& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
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
