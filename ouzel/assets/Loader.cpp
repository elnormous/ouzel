// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cctype>
#include <algorithm>
#include "Loader.hpp"
#include "Cache.hpp"

namespace ouzel
{
    namespace assets
    {
        Loader::Loader(uint32_t initType, const std::vector<std::string>& initExtensions):
            type(initType), extensions(initExtensions)
        {
            for (std::string& extension : extensions)
                std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });
        }

        Loader::~Loader()
        {
            if (cache)
                cache->removeLoader(this);
        }
    } // namespace assets
} // namespace ouzel
