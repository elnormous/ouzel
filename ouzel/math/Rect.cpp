// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <cmath>
#include "Rect.hpp"

namespace ouzel
{
    bool Rect::intersect(const Rect& r1, const Rect& r2, Rect& dst)
    {
        float xmin = std::max(r1.position.x, r2.position.x);
        float xmax = std::min(r1.right(), r2.right());
        if (xmax > xmin)
        {
            float ymin = std::max(r1.position.y, r2.position.y);
            float ymax = std::min(r1.bottom(), r2.bottom());
            if (ymax > ymin)
            {
                dst.set(xmin, ymin, xmax - xmin, ymax - ymin);
                return true;
            }
        }

        dst.set(0, 0, 0, 0);
        return false;
    }

    void Rect::combine(const Rect& r1, const Rect& r2, Rect& dst)
    {
        dst.position.x = std::min(r1.position.x, r2.position.x);
        dst.position.y = std::min(r1.position.y, r2.position.y);
        dst.size.width = std::max(r1.position.x + r1.size.width, r2.position.x + r2.size.width) - dst.position.x;
        dst.size.height = std::max(r1.position.y + r1.size.height, r2.position.y + r2.size.height) - dst.position.y;
    }
}
