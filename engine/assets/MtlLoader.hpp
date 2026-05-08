#ifndef FLUXION_ASSETS_MTLLOADER_HPP
#define FLUXION_ASSETS_MTLLOADER_HPP

#include "Bundle.hpp"

namespace fluxion::assets
{
    bool loadMtl(Cache& cache,
                 Bundle& bundle,
                 const Uuid& uuid,
                 const std::string& name,
                 const std::vector<std::byte>& data,
                 const Asset::Options& options);
}

#endif // FLUXION_ASSETS_MTLLOADER_HPP
