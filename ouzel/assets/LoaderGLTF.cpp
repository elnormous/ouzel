// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderGLTF.hpp"
#include "Bundle.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderGLTF::LoaderGLTF(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool LoaderGLTF::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            return true;
        }
    } // namespace assets
} // namespace ouzel
