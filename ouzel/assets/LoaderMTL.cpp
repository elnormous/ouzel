// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderMTL.hpp"
#include "Cache.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderMTL::LoaderMTL():
            Loader({"mtl"})
        {
        }

        bool LoaderMTL::loadAsset(const std::string& filename)
        {
            return false;
        }
    } // namespace assets
} // namespace ouzel
