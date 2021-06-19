// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_SPRITELOADER_HPP
#define OUZEL_ASSETS_SPRITELOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class SpriteLoader final: public Loader
    {
    public:
        explicit SpriteLoader();
        bool loadAsset(Cache& cache,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options& options) override;
    };
}

#endif // OUZEL_ASSETS_SPRITELOADER_HPP
