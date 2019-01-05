// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Quaternion.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    template<class T>
    float Quaternion<T>::getNorm()
    {
        float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        if (n == 1.0F) // already normalized
            return 1.0F;

        return sqrtf(n);
    }

    template<class T>
    void Quaternion<T>::normalize()
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

    template<class T>
    void Quaternion<T>::rotate(float angle, Vector3<T> axis)
    {
        axis.normalize();

        float cosAngle = cosf(angle / 2.0F);
        float sinAngle = sinf(angle / 2.0F);

        v[0] = axis.v[0] * sinAngle;
        v[1] = axis.v[1] * sinAngle;
        v[2] = axis.v[2] * sinAngle;
        v[3] = cosAngle;
    }

    template<class T>
    void Quaternion<T>::getRotation(float& angle, Vector3<T>& axis)
    {
        angle = 2.0F * acosf(v[3]);
        float s = sqrtf(1.0F - v[3] * v[3]);
        if (s < std::numeric_limits<float>::min()) // too close to zero
        {
            axis.v[0] = v[0];
            axis.v[1] = v[1];
            axis.v[2] = v[2];
        }
        else
        {
            axis.v[0] = v[0] / s;
            axis.v[1] = v[1] / s;
            axis.v[2] = v[2] / s;
        }
    }

    template<class T>
    void Quaternion<T>::setEulerAngles(const Vector3<T>& angles)
    {
        float angle;

        angle = angles.v[0] * 0.5F;
        const float sr = sinf(angle);
        const float cr = cosf(angle);

        angle = angles.v[1] * 0.5F;
        const float sp = sinf(angle);
        const float cp = cosf(angle);

        angle = angles.v[2] * 0.5F;
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

    template class Quaternion<float>;
}
