// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include <cassert>
#include "Vector3.h"
#include "MathUtils.h"

namespace ouzel
{
    Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
    Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);
    Vector3 Vector3::UNIT_X(1.0f, 0.0f, 0.0f);
    Vector3 Vector3::UNIT_Y(0.0f, 1.0f, 0.0f);
    Vector3 Vector3::UNIT_Z(0.0f, 0.0f, 1.0f);

    Vector3::Vector3():
        x(0.0f), y(0.0f), z(0.0f)
    {
    }

    Vector3::Vector3(float pX, float pY, float pZ):
        x(pX), y(pY), z(pZ)
    {
    }

    Vector3::Vector3(const float* array)
    {
        set(array);
    }

    Vector3::Vector3(const Vector3& p1, const Vector3& p2)
    {
        set(p1, p2);
    }

    Vector3::Vector3(const Vector3& copy)
    {
        set(copy);
    }

    Vector3::Vector3(const Vector2& v)
    {
        set(v.x, v.y, 0.0f);
    }

    Vector3& Vector3::operator=(const Vector2& v)
    {
        x = v.x;
        y = v.y;
        z = 0.0f;

        return *this;
    }

    Vector3 Vector3::fromColor(unsigned int color)
    {
        float components[3];
        int componentIndex = 0;
        for (int i = 2; i >= 0; --i)
        {
            int component = (color >> i*8) & 0x0000ff;

            components[componentIndex++] = static_cast<float>(component) / 255.0f;
        }

        Vector3 value(components);
        return value;
    }

    Vector3::~Vector3()
    {
    }

    float Vector3::angle(const Vector3& v1, const Vector3& v2)
    {
        float dx = v1.y * v2.z - v1.z * v2.y;
        float dy = v1.z * v2.x - v1.x * v2.z;
        float dz = v1.x * v2.y - v1.y * v2.x;

        return atan2f(sqrtf(dx * dx + dy * dy + dz * dz) + FLOAT_SMALL, dot(v1, v2));
    }

    void Vector3::add(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void Vector3::add(const Vector3& v1, const Vector3& v2, Vector3& dst)
    {
        dst.x = v1.x + v2.x;
        dst.y = v1.y + v2.y;
        dst.z = v1.z + v2.z;
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

    float Vector3::distanceSquared(const Vector3& v) const
    {
        float dx = v.x - x;
        float dy = v.y - y;
        float dz = v.z - z;

        return (dx * dx + dy * dy + dz * dz);
    }

    float Vector3::dot(const Vector3& v) const
    {
        return (x * v.x + y * v.y + z * v.z);
    }

    float Vector3::dot(const Vector3& v1, const Vector3& v2)
    {
        return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    }

    float Vector3::length() const
    {
        return sqrtf(x * x + y * y + z * z);
    }

    float Vector3::lengthSquared() const
    {
        return (x * x + y * y + z * z);
    }

    void Vector3::negate()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    Vector3& Vector3::normalize()
    {
        normalize(*this);
        return *this;
    }

    void Vector3::normalize(Vector3& dst) const
    {
        if (&dst != this)
        {
            dst.x = x;
            dst.y = y;
            dst.z = z;
        }

        float n = x * x + y * y + z * z;
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
    }

    void Vector3::scale(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    void Vector3::set(float pX, float pY, float pZ)
    {
        x = pX;
        y = pY;
        z = pZ;
    }

    void Vector3::set(const float* array)
    {
        assert(array);

        x = array[0];
        y = array[1];
        z = array[2];
    }

    void Vector3::set(const Vector3& v)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
    }

    void Vector3::set(const Vector3& p1, const Vector3& p2)
    {
        x = p2.x - p1.x;
        y = p2.y - p1.y;
        z = p2.z - p1.z;
    }

    void Vector3::subtract(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    void Vector3::subtract(const Vector3& v1, const Vector3& v2, Vector3& dst)
    {
        dst.x = v1.x - v2.x;
        dst.y = v1.y - v2.y;
        dst.z = v1.z - v2.z;
    }

    void Vector3::smooth(const Vector3& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
        {
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }
    }
}
