// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cfloat>
#include <cmath>

namespace ouzel
{
    class Vector4;

    class Plane
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

        static inline bool makeFrustumPlane(float a, float b, float c, float d, Plane& plane)
        {
            float n = sqrtf(a * a + b * b + c * c);
            if (n < FLT_EPSILON)
            {
                return false;
            }

            n = 1.0F / n;
            plane = Plane(a * n, b * n, c * n, d * n);

            return true;
        }
    };
}
