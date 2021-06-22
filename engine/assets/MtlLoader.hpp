// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_MTLLOADER_HPP
#define OUZEL_ASSETS_MTLLOADER_HPP

#include "Loader.hpp"

namespace ouzel::assets
{
    class MtlLoader final: public Loader
    {
    public:
        explicit MtlLoader(): Loader{Asset::Type::material} {}
        bool loadAsset(Cache& cache,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options& options) override;
    };
}

#endif // OUZEL_ASSETS_MTLLOADER_HPP
