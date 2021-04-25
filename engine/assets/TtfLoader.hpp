// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_TTFLOADER_HPP
#define OUZEL_ASSETS_TTFLOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class TtfLoader final: public Loader
    {
    public:
        explicit TtfLoader();
        bool loadAsset(Cache& cache,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       bool mipmaps = true) override;
    };
}

#endif // OUZEL_ASSETS_TTFLOADER_HPP
