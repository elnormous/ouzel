// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cctype>
#include <algorithm>
#include "Loader.hpp"
#include "Cache.hpp"

namespace ouzel
{
    namespace assets
    {
        Loader::Loader(uint32_t initType):
            type(initType)
        {
        }

        Loader::~Loader()
        {
            if (cache)
                cache->removeLoader(this);
        }
    } // namespace assets
} // namespace ouzel
