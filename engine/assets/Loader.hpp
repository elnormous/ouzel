// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_LOADER_HPP
#define OUZEL_ASSETS_LOADER_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace ouzel::assets
{
    class Cache;
    class Bundle;

    class Loader
    {
    public:
        enum class Type
        {
            bank,
            font,
            staticMesh,
            skinnedMesh,
            image,
            material,
            particleSystem,
            sprite,
            sound,
            cue
        };

        Loader(Cache& initCache, Type initType):
            cache(initCache), type(initType)
        {
        }

        virtual ~Loader() = default;

        Loader(const Loader&) = delete;
        Loader& operator=(const Loader&) = delete;

        Loader(Loader&&) = delete;
        Loader& operator=(Loader&&) = delete;

        auto getType() const noexcept { return type; }

        virtual bool loadAsset(Bundle& bundle,
                               const std::string& name,
                               const std::vector<std::byte>& data,
                               bool mipmaps = true) = 0;

    protected:
        Cache& cache;
        Type type;
    };
}

#endif // OUZEL_ASSETS_LOADER_HPP
