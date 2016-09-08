// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererVoid;
        class RenderTargetVoid;

        class TextureVoid: public Texture
        {
            friend RendererVoid;
            friend RenderTargetVoid;
        public:
            virtual ~TextureVoid();

        protected:
            TextureVoid();
            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
