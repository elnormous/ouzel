// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_IMAGELOADER_HPP
#define OUZEL_ASSETS_IMAGELOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class ImageLoader final: public Loader
    {
    public:
        explicit ImageLoader();
        bool loadAsset(Cache& cache,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options& options) override;
    };
}

#endif // OUZEL_ASSETS_IMAGELOADER_HPP
