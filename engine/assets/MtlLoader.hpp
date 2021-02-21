// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_MTLLOADER_HPP
#define OUZEL_ASSETS_MTLLOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class MtlLoader final: public Loader
    {
    public:
        explicit MtlLoader(Cache& initCache);
        bool loadAsset(Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       bool mipmaps = true) final;
    };
}

#endif // OUZEL_ASSETS_MTLLOADER_HPP
