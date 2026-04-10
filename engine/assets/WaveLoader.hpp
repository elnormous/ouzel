#ifndef FLUXION_ASSETS_WAVELOADER_HPP
#define FLUXION_ASSETS_WAVELOADER_HPP

#include "Bundle.hpp"

namespace fluxion::assets
{
    bool loadWave(Cache& cache,
                  Bundle& bundle,
                  const std::string& name,
                  const std::vector<std::byte>& data,
                  const Asset::Options& options);
}

#endif // FLUXION_ASSETS_WAVELOADER_HPP
