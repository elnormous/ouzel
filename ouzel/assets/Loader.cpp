// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cctype>
#include <algorithm>
#include "Loader.hpp"
#include "Cache.hpp"

namespace ouzel
{
    namespace assets
    {
        Loader::Loader(const std::vector<std::string>& initExtensions):
            extensions(initExtensions)
        {
            for (std::string& extension : extensions)
            {
                std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c){ return std::tolower(c); });
            }
        }

        Loader::~Loader()
        {
            if (cache)
            {
                cache->removeLoader(this);
            }
        }
    } // namespace assets
} // namespace ouzel
