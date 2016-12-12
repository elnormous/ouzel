// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cmath>
#include "Quaternion.h"
#include "MathUtils.h"

namespace ouzel
{
    const Quaternion Quaternion::IDENTITY(0.0f, 0.0f, 0.0f, 1.0f);

    const Quaternion Quaternion::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

    Quaternion& Quaternion::normalize()
    {
        float n = x * x + y * y + z * z + w * w;
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
        w *= n;

        return *this;
    }

    void Quaternion::rotate(float angle, Vector3 axis)
    {
        axis.normalize();

        float cosAngle = cosf(angle / 2.0f);
        float sinAngle = sinf(angle / 2.0f);

        x = axis.x * sinAngle;
        y = axis.y * sinAngle;
        z = axis.z * sinAngle;
        w = cosAngle;
    }

    void Quaternion::setEulerAngles(const Vector3& angles)
    {
        float angle;

        angle = angles.x * 0.5;
        const float sr = sinf(angle);
        const float cr = cosf(angle);

        angle = angles.y * 0.5;
        const float sp = sinf(angle);
        const float cp = cosf(angle);

        angle = angles.z * 0.5;
        const float sy = sinf(angle);
        const float cy = cosf(angle);

        const float cpcy = cp * cy;
        const float spcy = sp * cy;
        const float cpsy = cp * sy;
        const float spsy = sp * sy;

        x = sr * cpcy - cr * spsy;
        y = cr * spcy + sr * cpsy;
        z = cr * cpsy - sr * spcy;
        w = cr * cpcy + sr * spsy;
    }
    
    Vector3 Quaternion::getEulerAngles() const
    {
        Vector3 result;

        result.x = atan2f(2.0f * (y * z + w * x), w * w - x * x - y * y + z * z);
        result.y = asinf(-2.0f * (x * z - w * y));
        result.z = atan2f(2.0f * (x * y + w * z), w * w + x * x - y * y - z * z);
        return result;
    }

    float Quaternion::getEulerAngleX() const
    {
        return atan2f(2.0f * (y * z + w * x), w * w - x * x - y * y + z * z);
    }

    float Quaternion::getEulerAngleY() const
    {
        return asinf(-2.0f * (x * z - w * y));
    }

    float Quaternion::getEulerAngleZ() const
    {
        return atan2f(2.0f * (x * y + w * z), w * w + x * x - y * y - z * z);
    }
}
