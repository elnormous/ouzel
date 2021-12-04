// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_LOADER_HPP
#define OUZEL_ASSETS_LOADER_HPP

#include <cstddef>
#include <string>
#include <vector>
#include "Asset.hpp"

namespace ouzel::assets
{
    class Cache;
    class Bundle;

    class Loader
    {
    public:
        Loader(Asset::Type initType) noexcept: type{initType} {}

        virtual ~Loader() = default;

        Loader(const Loader&) = delete;
        Loader& operator=(const Loader&) = delete;

        Loader(Loader&&) = delete;
        Loader& operator=(Loader&&) = delete;

        auto getType() const noexcept { return type; }

        virtual bool loadAsset(Cache& cache,
                               Bundle& bundle,
                               const std::string& name,
                               const std::vector<std::byte>& data,
                               const Asset::Options& options) = 0;

    private:
        Asset::Type type;
    };
}

#endif // OUZEL_ASSETS_LOADER_HPP
