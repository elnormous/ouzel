// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "LoaderGLTF.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderGLTF::LoaderGLTF():
            Loader(TYPE, {"gltf"})
        {
        }

        bool LoaderGLTF::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            return true;
        }
    } // namespace assets
} // namespace ouzel
