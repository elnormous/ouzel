// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cmath>
#include "Rect.hpp"

namespace ouzel
{
    template<class T>
    bool Rect<T>::intersect(const Rect& r1, const Rect& r2, Rect& dst)
    {
        T xmin = std::max(r1.position.v[0], r2.position.v[0]);
        T xmax = std::min(r1.right(), r2.right());
        if (xmax > xmin)
        {
            T ymin = std::max(r1.position.v[1], r2.position.v[1]);
            T ymax = std::min(r1.bottom(), r2.bottom());
            if (ymax > ymin)
            {
                dst.set(xmin, ymin, xmax - xmin, ymax - ymin);
                return true;
            }
        }

        dst.set(0, 0, 0, 0);
        return false;
    }

    template<class T>
    void Rect<T>::combine(const Rect& r1, const Rect& r2, Rect& dst)
    {
        dst.position.v[0] = std::min(r1.position.v[0], r2.position.v[0]);
        dst.position.v[1] = std::min(r1.position.v[1], r2.position.v[1]);
        dst.size.width = std::max(r1.position.v[0] + r1.size.width, r2.position.v[0] + r2.size.width) - dst.position.v[0];
        dst.size.height = std::max(r1.position.v[1] + r1.size.height, r2.position.v[1] + r2.size.height) - dst.position.v[1];
    }

    template class Rect<float>;
}
