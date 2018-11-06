// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef LOADERIMAGE_HPP
#define LOADERIMAGE_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderImage final: public Loader
        {
        public:
            static const uint32_t TYPE = Loader::IMAGE;

            explicit LoaderImage(Cache& initCache);
            bool loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel

#endif // LOADERIMAGE_HPP
