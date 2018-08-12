// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cctype>
#include <algorithm>
#include "Loader.hpp"
#include "Cache.hpp"

namespace ouzel
{
    namespace assets
    {
        Loader::Loader(Cache& initCache, uint32_t initType):
            cache(initCache), type(initType)
        {
            cache.addLoader(this);
        }

        Loader::~Loader()
        {
            cache.removeLoader(this);
        }
    } // namespace assets
} // namespace ouzel
