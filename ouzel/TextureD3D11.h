// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include "Texture.h"

namespace ouzel
{
    class RendererD3D11;
    class RenderTargetD3D11;

    class TextureD3D11: public Texture
    {
        friend RendererD3D11;
        friend RenderTargetD3D11;
    public:
        virtual ~TextureD3D11();

        virtual bool init(Size2 const& size, bool dynamic, bool mipmaps = true) override;
        virtual bool initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;

        virtual bool upload(const void* data, const Size2& size) override;

        ID3D11Texture2D* getTexture() const { return _texture; }
        ID3D11ShaderResourceView* getResourceView() const { return _resourceView; }

    protected:
        TextureD3D11();

        void clean();
        bool createTexture(const void* data, UINT width, UINT height);
        bool uploadData(const void* data, UINT width, UINT height);

        ID3D11Texture2D* _texture = nullptr;
        ID3D11ShaderResourceView* _resourceView = nullptr;

        UINT _width = 0;
        UINT _height = 0;
    };
}
