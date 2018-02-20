// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <vector>
#include "SpriteFrame.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        struct SpriteData
        {
            bool init(const std::string& filename, bool mipmaps = true);
            bool init(const std::vector<uint8_t>& data, bool mipmaps = true);

            struct Animation
            {
                std::string name;
                std::vector<SpriteFrame> frames;
            };

            std::map<std::string, Animation> animations;
            std::shared_ptr<graphics::Texture> texture;
        };
    } // namespace scene
} // namespace ouzel
