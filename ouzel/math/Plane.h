// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Plane
    {
    public:
        float v[4];

        Plane()
        {
        }

        Plane(float x, float y, float z, float w):
            v{x, y, z, w}
        {
        }

        void flip()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }
    };
}
