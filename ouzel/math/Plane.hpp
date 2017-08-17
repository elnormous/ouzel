// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Vector4;

    class Plane
    {
    public:
        union
        {
            struct
            {
                float a;
                float b;
                float c;
                float d;
            };
            float v[4];
        };

        Plane():
            a(0.0f), b(0.0f), c(0.0f), d(0.0f)
        {
        }

        Plane(float aA, float aB, float aC, float aD):
            a(aA), b(aB), c(aC), d(aD)
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
