// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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
                NONE,
                FONT,
                MESH,
                SKINNED_MESH,
                IMAGE,
                MATERIAL,
                PARTICLE_SYSTEM,
                SPRITE,
                SOUND
            };

            Loader(Cache& initCache, uint32_t initType);
            virtual ~Loader();

            Loader(const Loader&) = delete;
            Loader& operator=(const Loader&) = delete;

            Loader(Loader&&) = delete;
            Loader& operator=(Loader&&) = delete;

            inline uint32_t getType() const { return type; }

            virtual bool loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) = 0;

        protected:
            Cache& cache;
            uint32_t type = Type::NONE;
        };
    } // namespace assets
} // namespace ouzel
