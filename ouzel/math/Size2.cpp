// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Size2.hpp"
#include "Size3.hpp"

namespace ouzel
{
    template<class T>
    Size2<T>::Size2(const Size3<T>& s):
        width(s.width), height(s.height)
    {
    }

    template<class T>
    Size2<T>& Size2<T>::operator=(const Size3<T>& s)
    {
        width = s.width;
        height = s.height;
        return *this;
    }

    template class Size2<float>;
}
