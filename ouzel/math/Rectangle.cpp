// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <cmath>
#include <cassert>
#include "Rectangle.hpp"

namespace ouzel
{
    bool Rectangle::intersect(const Rectangle& r1, const Rectangle& r2, Rectangle* dst)
    {
        assert(dst);

        float xmin = std::max(r1.position.v[0], r2.position.v[0]);
        float xmax = std::min(r1.right(), r2.right());
        if (xmax > xmin)
        {
            float ymin = std::max(r1.position.v[1], r2.position.v[1]);
            float ymax = std::min(r1.bottom(), r2.bottom());
            if (ymax > ymin)
            {
                dst->set(xmin, ymin, xmax - xmin, ymax - ymin);
                return true;
            }
        }

        dst->set(0, 0, 0, 0);
        return false;
    }

    void Rectangle::combine(const Rectangle& r1, const Rectangle& r2, Rectangle* dst)
    {
        assert(dst);

        dst->position.v[0] = std::min(r1.position.v[0], r2.position.v[0]);
        dst->position.v[1] = std::min(r1.position.v[1], r2.position.v[1]);
        dst->size.v[0] = std::max(r1.position.v[0] + r1.size.v[0], r2.position.v[0] + r2.size.v[0]) - dst->position.v[0];
        dst->size.v[1] = std::max(r1.position.v[1] + r1.size.v[1], r2.position.v[1] + r2.size.v[1]) - dst->position.v[1];
    }
}
