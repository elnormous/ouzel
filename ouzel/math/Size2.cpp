// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cstdint>
#include "Size2.hpp"
#include "Size3.hpp"

namespace ouzel
{
    template<class T>
    Size2<T>::Size2(const Size3<T>& s):
        v{s.v[0], s.v[1]}
    {
    }

    template class Size2<float>;
    template class Size2<uint32_t>;
}
