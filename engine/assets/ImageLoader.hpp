#ifndef FLUXION_ASSETS_IMAGELOADER_HPP
#define FLUXION_ASSETS_IMAGELOADER_HPP

#include "Bundle.hpp"

namespace fluxion::assets
{
    bool loadImage(Cache& cache,
                   Bundle& bundle,
                   const Uuid& uuid,
                   const std::string& name,
                   const std::vector<std::byte>& data,
                   const Asset::Options& options);
}

#endif // FLUXION_ASSETS_IMAGELOADER_HPP
