// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderGLTF.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderGLTF::LoaderGLTF():
            Loader(TYPE)
        {
        }

        bool LoaderGLTF::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            return true;
        }
    } // namespace assets
} // namespace ouzel
