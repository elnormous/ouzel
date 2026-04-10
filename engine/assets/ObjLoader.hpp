#ifndef FLUXION_ASSETS_OBJLOADER_HPP
#define FLUXION_ASSETS_OBJLOADER_HPP

#include "Bundle.hpp"

namespace fluxion::assets
{
    bool loadObj(Cache& cache,
                 Bundle& bundle,
                 const std::string& name,
                 const std::vector<std::byte>& data,
                 const Asset::Options& options);
}

#endif // FLUXION_ASSETS_OBJLOADER_HPP
