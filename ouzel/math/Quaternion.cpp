// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include <cmath>
#include "Quaternion.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    float Quaternion::getNorm()
    {
        float n = x * x + y * y + z * z + w * w;
        if (n == 1.0F) // already normalized
            return 1.0F;

        return sqrtf(n);
    }

    void Quaternion::normalize()
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

    void Quaternion::rotate(float angle, Vector3 axis)
    {
        axis.normalize();

        float cosAngle = cosf(angle / 2.0F);
        float sinAngle = sinf(angle / 2.0F);

        x = axis.x * sinAngle;
        y = axis.y * sinAngle;
        z = axis.z * sinAngle;
        w = cosAngle;
    }

    void Quaternion::getRotation(float& angle, Vector3& axis)
    {
        angle = 2.0F * acosf(w);
        float s = sqrtf(1.0F - w * w);
        if (s < EPSILON) // too close to zero
        {
            axis.x = x;
            axis.y = y;
            axis.z = z;
        }
        else
        {
            axis.x = x / s;
            axis.y = y / s;
            axis.z = z / s;
        }
    }

    void Quaternion::setEulerAngles(const Vector3& angles)
    {
        float angle;

        angle = angles.x * 0.5F;
        const float sr = sinf(angle);
        const float cr = cosf(angle);

        angle = angles.y * 0.5F;
        const float sp = sinf(angle);
        const float cp = cosf(angle);

        angle = angles.z * 0.5F;
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

        result.x = atan2f(2.0F * (y * z + w * x), w * w - x * x - y * y + z * z);
        result.y = asinf(-2.0F * (x * z - w * y));
        result.z = atan2f(2.0F * (x * y + w * z), w * w + x * x - y * y - z * z);
        return result;
    }

    float Quaternion::getEulerAngleX() const
    {
        return atan2f(2.0F * (y * z + w * x), w * w - x * x - y * y + z * z);
    }

    float Quaternion::getEulerAngleY() const
    {
        return asinf(-2.0F * (x * z - w * y));
    }

    float Quaternion::getEulerAngleZ() const
    {
        return atan2f(2.0F * (x * y + w * z), w * w + x * x - y * y - z * z);
    }
}
