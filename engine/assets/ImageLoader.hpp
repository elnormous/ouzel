// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_IMAGELOADER_HPP
#define OUZEL_ASSETS_IMAGELOADER_HPP

#include "Bundle.hpp"

namespace ouzel::assets
{
    bool loadImage(Cache& cache,
                   Bundle& bundle,
                   const std::string& name,
                   const std::vector<std::byte>& data,
                   const Asset::Options& options);
}

#endif // OUZEL_ASSETS_IMAGELOADER_HPP
