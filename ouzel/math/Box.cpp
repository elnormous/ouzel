// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Box.hpp"

namespace ouzel
{
    template<class T>
    Box3<T>::Box3(const Box2<T>& box):
        min(box.min), max(box.max)
    {
    }

    template class Box3<float>;
}
