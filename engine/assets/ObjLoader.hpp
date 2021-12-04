// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_OBJLOADER_HPP
#define OUZEL_ASSETS_OBJLOADER_HPP

#include "Bundle.hpp"

namespace ouzel::assets
{
    bool loadObj(Cache& cache,
                 Bundle& bundle,
                 const std::string& name,
                 const std::vector<std::byte>& data,
                 const Asset::Options& options);
}

#endif // OUZEL_ASSETS_OBJLOADER_HPP
