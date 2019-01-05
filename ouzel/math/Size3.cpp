// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Size3.hpp"
#include "Size2.hpp"

namespace ouzel
{
    template<class T>
    Size3<T>::Size3(const Size2<T>& s):
        width(s.width), height(s.height)
    {
    }

    template<class T>
    Size3<T>& Size3<T>::operator=(const Size2<T>& s)
    {
        width = s.width;
        height = s.height;
        depth = 0;
        return *this;
    }

    template class Size3<float>;
}
