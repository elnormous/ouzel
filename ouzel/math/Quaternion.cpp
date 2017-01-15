// Copyright (C) 2017 Elviss Strazdins
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
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        // Already normalized.
        if (n == 1.0f)
            return *this;

        n = sqrtf(n);
        // Too close to zero.
        if (n < TOLERANCE)
            return *this;

        n = 1.0f / n;
        v[0] *= n;
        v[1] *= n;
        v[2] *= n;
        v[3] *= n;

        return *this;
    }

    void Quaternion::rotate(float angle, Vector3 axis)
    {
        axis.normalize();

        float cosAngle = cosf(angle / 2.0f);
        float sinAngle = sinf(angle / 2.0f);

        v[0] = axis.v[0] * sinAngle;
        v[1] = axis.v[1] * sinAngle;
        v[2] = axis.v[2] * sinAngle;
        v[3] = cosAngle;
    }

    void Quaternion::setEulerAngles(const Vector3& angles)
    {
        float angle;

        angle = angles.v[0] * 0.5f;
        const float sr = sinf(angle);
        const float cr = cosf(angle);

        angle = angles.v[1] * 0.5f;
        const float sp = sinf(angle);
        const float cp = cosf(angle);

        angle = angles.v[2] * 0.5f;
        const float sy = sinf(angle);
        const float cy = cosf(angle);

        const float cpcy = cp * cy;
        const float spcy = sp * cy;
        const float cpsy = cp * sy;
        const float spsy = sp * sy;

        v[0] = sr * cpcy - cr * spsy;
        v[1] = cr * spcy + sr * cpsy;
        v[2] = cr * cpsy - sr * spcy;
        v[3] = cr * cpcy + sr * spsy;
    }

    Vector3 Quaternion::getEulerAngles() const
    {
        Vector3 result;

        result.v[0] = atan2f(2.0f * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
        result.v[1] = asinf(-2.0f * (v[0] * v[2] - v[3] * v[1]));
        result.v[2] = atan2f(2.0f * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
        return result;
    }

    float Quaternion::getEulerAngleX() const
    {
        return atan2f(2.0f * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
    }

    float Quaternion::getEulerAngleY() const
    {
        return asinf(-2.0f * (v[0] * v[2] - v[3] * v[1]));
    }

    float Quaternion::getEulerAngleZ() const
    {
        return atan2f(2.0f * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
    }
}
