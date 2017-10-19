// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "SpriteFrame.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        struct SpriteDefinition
        {
            static bool load(const std::string& filename, bool mipmaps, SpriteDefinition& spriteDefinition);

            std::vector<SpriteFrame> frames;
            std::shared_ptr<graphics::Texture> texture;
        };
    } // namespace scene
} // namespace ouzel
