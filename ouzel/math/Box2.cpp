// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Box2.hpp"

namespace ouzel
{
    template<class T>
    void Box2<T>::getCorners(Vector2<T>* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis

        // left-bottom
        dst[0].v[0] = min.v[0];
        dst[0].v[1] = min.v[1];

        // right-bottom
        dst[1].v[0] = max.v[0];
        dst[1].v[1] = min.v[1];

        // right-top
        dst[2].v[0] = max.v[0];
        dst[2].v[1] = max.v[1];

        // left-top
        dst[3].v[0] = min.v[0];
        dst[3].v[1] = max.v[1];
    }

    template<class T>
    void Box2<T>::merge(const Box2& box)
    {
        // Calculate the new minimum point
        min.v[0] = std::min(min.v[0], box.min.v[0]);
        min.v[1] = std::min(min.v[1], box.min.v[1]);

        // Calculate the new maximum point
        max.v[0] = std::max(max.v[0], box.max.v[0]);
        max.v[1] = std::max(max.v[1], box.max.v[1]);
    }

    template class Box2<float>;
}
