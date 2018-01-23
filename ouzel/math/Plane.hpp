// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Vector4;

    class Plane
    {
    public:
        float a = 0.0f;
        float b = 0.0f;
        float c = 0.0f;
        float d = 0.0f;

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
    };
}
