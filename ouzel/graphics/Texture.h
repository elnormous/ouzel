// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class TextureResource;

        class Texture: public Noncopyable
        {
        public:
            Texture();

        private:
            std::unique_ptr<TextureResource> resource;
        };
    } // namespace graphics
} // namespace ouzel
