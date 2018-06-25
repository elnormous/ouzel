// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderGLTF: public Loader
        {
        public:
            static const uint32_t TYPE = Loader::SKINNED_MESH;

            LoaderGLTF();
            virtual bool loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel
