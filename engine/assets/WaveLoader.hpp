// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_WAVELOADER_HPP
#define OUZEL_ASSETS_WAVELOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class WaveLoader final: public Loader
    {
    public:
        explicit WaveLoader(Cache& initCache);
        bool loadAsset(Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       bool mipmaps = true) final;
    };
}

#endif // OUZEL_ASSETS_WAVELOADER_HPP
