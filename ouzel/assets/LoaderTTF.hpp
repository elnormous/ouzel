// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderTTF: public Loader
        {
        public:
            static const uint32_t TYPE = Loader::FONT;

            LoaderTTF();
            virtual bool loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel
