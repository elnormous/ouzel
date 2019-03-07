// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "GltfLoader.hpp"
#include "Bundle.hpp"

namespace ouzel
{
    namespace assets
    {
        GltfLoader::GltfLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool GltfLoader::loadAsset(Bundle& bundle,
                                   const std::string& name,
                                   const std::vector<uint8_t>& data,
                                   bool mipmaps)
        {
            return true;
        }
    } // namespace assets
} // namespace ouzel
