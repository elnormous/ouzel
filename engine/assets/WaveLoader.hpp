// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_WAVELOADER_HPP
#define OUZEL_ASSETS_WAVELOADER_HPP

#include "Bundle.hpp"

namespace ouzel::assets
{
    bool loadWave(Cache& cache,
                  Bundle& bundle,
                  const std::string& name,
                  const std::vector<std::byte>& data,
                  const Asset::Options& options);
}

#endif // OUZEL_ASSETS_WAVELOADER_HPP
