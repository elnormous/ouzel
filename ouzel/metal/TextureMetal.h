// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Texture.h"

namespace ouzel
{
    namespace video
    {
        class RendererMetal;
        class RenderTargetMetal;

        class TextureMetal: public Texture
        {
            friend RendererMetal;
            friend RenderTargetMetal;
        public:
            virtual ~TextureMetal();

            virtual bool init(const Size2& size, bool dynamic, bool mipmaps = true) override;
            virtual bool initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;

            virtual bool upload(const void* data, const Size2& size) override;

        protected:
            TextureMetal();
        };
    } // namespace video
} // namespace ouzel
