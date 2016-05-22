// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <d3d11.h>
#include "Texture.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11;
        class RenderTargetD3D11;

        class TextureD3D11: public Texture
        {
            friend RendererD3D11;
            friend RenderTargetD3D11;
        public:
            virtual ~TextureD3D11();
            virtual void free() override;

            virtual bool init(const Size2& newSize, bool newDynamic, bool newMipmaps = true, bool newRenderTarget = false) override;
            virtual bool initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps = true) override;

            virtual bool uploadMipmap(uint32_t level, const void* data) override;

            ID3D11Texture2D* getTexture() const { return texture; }
            ID3D11ShaderResourceView* getResourceView() const { return resourceView; }

        protected:
            TextureD3D11();

            bool createTexture(UINT newWidth, UINT newHeight);
            bool uploadData(const void* data, const Size2& newSize);

            ID3D11Texture2D* texture = nullptr;
            ID3D11ShaderResourceView* resourceView = nullptr;

            UINT width = 0;
            UINT height = 0;
        };
    } // namespace graphics
} // namespace ouzel
