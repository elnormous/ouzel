// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_COLLADALOADER_HPP
#define OUZEL_ASSETS_COLLADALOADER_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class ColladaLoader final: public Loader
        {
        public:
            static constexpr uint32_t TYPE = Loader::SkinnedMesh;

            explicit ColladaLoader(Cache& initCache);
            bool loadAsset(Bundle& bundle,
                           const std::string& name,
                           const std::vector<uint8_t>& data,
                           bool mipmaps = true) final;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_COLLADALOADER_HPP
