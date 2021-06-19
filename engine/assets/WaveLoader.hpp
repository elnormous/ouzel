// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_WAVELOADER_HPP
#define OUZEL_ASSETS_WAVELOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class WaveLoader final: public Loader
    {
    public:
        explicit WaveLoader();
        bool loadAsset(Cache& cache,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options& options) override;
    };
}

#endif // OUZEL_ASSETS_WAVELOADER_HPP
