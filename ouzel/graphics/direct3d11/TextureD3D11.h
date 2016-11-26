// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "graphics/Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RenderTargetD3D11;

        class TextureD3D11: public Texture
        {
            friend RenderTargetD3D11;
        public:
            TextureD3D11();
            virtual ~TextureD3D11();
            virtual void free() override;

            ID3D11Texture2D* getTexture() const { return texture; }
            ID3D11ShaderResourceView* getResourceView() const { return resourceView; }

        protected:
            virtual bool upload() override;

            ID3D11Texture2D* texture = nullptr;
            ID3D11ShaderResourceView* resourceView = nullptr;

            UINT width = 0;
            UINT height = 0;
        };
    } // namespace graphics
} // namespace ouzel
