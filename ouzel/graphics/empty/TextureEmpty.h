// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererEmpty;
        class RenderTargetEmpty;

        class TextureEmpty: public Texture
        {
            friend RendererEmpty;
            friend RenderTargetEmpty;
        public:
            virtual ~TextureEmpty();

        protected:
            TextureEmpty();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
