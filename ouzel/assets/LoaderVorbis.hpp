// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_LOADERVORBIS_HPP
#define OUZEL_LOADERVORBIS_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderVorbis final: public Loader
        {
        public:
            static const uint32_t TYPE = Loader::SOUND;

            explicit LoaderVorbis(Cache& initCache);
            bool loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_LOADERVORBIS_HPP
