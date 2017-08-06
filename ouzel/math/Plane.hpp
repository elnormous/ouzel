// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Vector4;

    class Plane
    {
    public:
        float v[4];

        Plane()
        {
        }

        Plane(float aA, float aB, float aC, float aD)
        {
            v[0] = aA;
            v[1] = aB;
            v[2] = aC;
            v[3] = aD;
        }

        void flip()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }

        float dot(const Vector4& vec) const;
        void normalize();

        inline bool operator==(const Plane& plane) const
        {
            return v[0] == plane.v[0] && v[1] == plane.v[1] && v[2] == plane.v[2] && v[3] == plane.v[3];
        }

        inline bool operator!=(const Plane& plane) const
        {
            return v[0] != plane.v[0] || v[1] != plane.v[1] || v[2] != plane.v[2] || v[3] != plane.v[3];
        }
    };
}
