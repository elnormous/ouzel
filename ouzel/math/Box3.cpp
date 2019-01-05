// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Box3.hpp"
#include "Box2.hpp"

namespace ouzel
{
    template<class T>
    Box3<T>::Box3(const Box2<T>& box):
        min(box.min), max(box.max)
    {
    }

    template<class T>
    Box3<T>& Box3<T>::operator=(const Box2<T>& box)
    {
        min = box.min;
        max = box.max;

        return *this;
    }

    template<class T>
    void Box3<T>::getCorners(Vector3<T>* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive v[2]-axis
        
        // left-bottom-front
        dst[0].v[0] = min.v[0];
        dst[0].v[1] = min.v[1];
        dst[0].v[2] = min.v[2];
        
        // right-bottom-front
        dst[1].v[0] = max.v[0];
        dst[1].v[1] = min.v[1];
        dst[1].v[2] = min.v[2];

        // right-top-front
        dst[2].v[0] = max.v[0];
        dst[2].v[1] = max.v[1];
        dst[2].v[2] = min.v[2];

        // left-top-front
        dst[3].v[0] = min.v[0];
        dst[3].v[1] = max.v[1];
        dst[3].v[2] = min.v[2];

        // left-bottom-back
        dst[4].v[0] = min.v[0];
        dst[4].v[1] = min.v[1];
        dst[4].v[2] = max.v[2];

        // right-bottom-back
        dst[5].v[0] = max.v[0];
        dst[5].v[1] = min.v[1];
        dst[5].v[2] = max.v[2];

        // right-top-back
        dst[6].v[0] = max.v[0];
        dst[6].v[1] = max.v[1];
        dst[6].v[2] = max.v[2];

        // left-top-back
        dst[7].v[0] = min.v[0];
        dst[7].v[1] = max.v[1];
        dst[7].v[2] = max.v[2];
    }

    template<class T>
    void Box3<T>::merge(const Box3& box)
    {
        // Calculate the new minimum point
        min.v[0] = std::min(min.v[0], box.min.v[0]);
        min.v[1] = std::min(min.v[1], box.min.v[1]);
        min.v[2] = std::min(min.v[2], box.min.v[2]);

        // Calculate the new maximum point
        max.v[0] = std::max(max.v[0], box.max.v[0]);
        max.v[1] = std::max(max.v[1], box.max.v[1]);
        max.v[2] = std::max(max.v[1], box.max.v[2]);
    }

    template class Box3<float>;
}
