// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ASSETS_LOADER_HPP
#define OUZEL_ASSETS_LOADER_HPP

#include <string>
#include <vector>

namespace ouzel
{
    namespace assets
    {
        class Cache;
        class Bundle;

        class Loader
        {
        public:
            enum Type
            {
                Font,
                StaticMesh,
                SkinnedMesh,
                Image,
                Material,
                ParticleSystem,
                Sprite,
                Sound,
                Bank
            };

            Loader(Cache& initCache, uint32_t initType):
                cache(initCache), type(initType)
            {
            }

            virtual ~Loader() {}

            Loader(const Loader&) = delete;
            Loader& operator=(const Loader&) = delete;

            Loader(Loader&&) = delete;
            Loader& operator=(Loader&&) = delete;

            inline auto getType() const noexcept { return type; }

            virtual bool loadAsset(Bundle& bundle,
                                   const std::string& name,
                                   const std::vector<uint8_t>& data,
                                   bool mipmaps = true) = 0;

        protected:
            Cache& cache;
            uint32_t type;
        };
    } // namespace assets
} // namespace ouzel

#endif // OUZEL_ASSETS_LOADER_HPP
