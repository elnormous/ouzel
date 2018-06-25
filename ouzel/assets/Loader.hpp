// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>

namespace ouzel
{
    namespace assets
    {
        class Cache;

        class Loader
        {
            friend Cache;
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

            Loader(uint32_t initType, const std::vector<std::string>& initExtensions);
            virtual ~Loader();

            Loader(const Loader&) = delete;
            Loader& operator=(const Loader&) = delete;

            Loader(Loader&&) = delete;
            Loader& operator=(Loader&&) = delete;

            inline uint32_t getType() const { return type; }

            virtual bool loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) = 0;

        protected:
            uint32_t type = Type::NONE;
            Cache* cache = nullptr;
            std::vector<std::string> extensions;
        };
    } // namespace assets
} // namespace ouzel
