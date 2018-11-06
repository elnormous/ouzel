// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_PLANE_HPP
#define OUZEL_PLANE_HPP

#include <cmath>
#include "MathUtils.hpp"

namespace ouzel
{
    class Vector4;

    class Plane final
    {
    public:
        float a = 0.0F;
        float b = 0.0F;
        float c = 0.0F;
        float d = 0.0F;

        Plane()
        {
        }

        Plane(float initA, float initB, float initC, float initD):
            a(initA), b(initB), c(initC), d(initD)
        {
        }

        void flip()
        {
            a = -a;
            b = -b;
            c = -c;
            d = -d;
        }

        float dot(const Vector4& vec) const;
        void normalize();

        inline bool operator==(const Plane& plane) const
        {
            return a == plane.a && b == plane.b && c == plane.c && d == plane.d;
        }

        inline bool operator!=(const Plane& plane) const
        {
            return a != plane.a || b != plane.b || c != plane.c || d != plane.d;
        }

        static inline Plane makeFrustumPlane(float a, float b, float c, float d)
        {
            float n = sqrtf(a * a + b * b + c * c);
            if (n < std::numeric_limits<float>::min()) // too close to zero
                return Plane();

            n = 1.0F / n;
            return Plane(a * n, b * n, c * n, d * n);
        }
    };
}

#endif // OUZEL_PLANE_HPP
