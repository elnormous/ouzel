// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_LOADERSPRITE_HPP
#define OUZEL_LOADERSPRITE_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderSprite final: public Loader
        {
        public:
            static const uint32_t TYPE = Loader::SPRITE;

            explicit LoaderSprite(Cache& initCache);
            bool loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_LOADERSPRITE_HPP
