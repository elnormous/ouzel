// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "SpriteFrame.hpp"

namespace ouzel
{
    namespace scene
    {
        struct SpriteDefinition
        {
            static SpriteDefinition loadSpriteDefinition(const std::string& filename, bool mipmaps = true);

            std::vector<SpriteFrame> frames;
            std::shared_ptr<graphics::Texture> texture;
        };
    } // namespace scene
} // namespace ouzel
