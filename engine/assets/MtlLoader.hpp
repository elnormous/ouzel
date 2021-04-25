// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_MTLLOADER_HPP
#define OUZEL_ASSETS_MTLLOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class MtlLoader final: public Loader
    {
    public:
        explicit MtlLoader();
        bool loadAsset(Cache& cache,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       bool mipmaps = true) override;
    };
}

#endif // OUZEL_ASSETS_MTLLOADER_HPP
