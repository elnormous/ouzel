// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_CUELOADER_HPP
#define OUZEL_ASSETS_CUELOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class CueLoader final: public Loader
    {
    public:
        explicit CueLoader();
        bool loadAsset(Cache& cache,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options& options) override;
    };
}

#endif // OUZEL_ASSETS_CUELOADER_HPP
