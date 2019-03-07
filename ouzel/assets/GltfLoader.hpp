// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_GLTFLOADER_HPP
#define OUZEL_ASSETS_GLTFLOADER_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class GltfLoader final: public Loader
        {
        public:
            static constexpr uint32_t TYPE = Loader::SKINNED_MESH;

            explicit GltfLoader(Cache& initCache);
            bool loadAsset(Bundle& bundle,
                           const std::string& name,
                           const std::vector<uint8_t>& data,
                           bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_GLTFLOADER_HPP
