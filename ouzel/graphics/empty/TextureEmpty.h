// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RenderTargetEmpty;

        class TextureEmpty: public Texture
        {
            friend RenderTargetEmpty;
        public:
            TextureEmpty();

        protected:
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
