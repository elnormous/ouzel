// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_LOADEROBJ_HPP
#define OUZEL_ASSETS_LOADEROBJ_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderOBJ final: public Loader
        {
        public:
            static constexpr uint32_t TYPE = Loader::STATIC_MESH;

            explicit LoaderOBJ(Cache& initCache);
            bool loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_LOADEROBJ_HPP
