// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <d3d11.h>
#include "Texture.h"

namespace ouzel
{
    class TextureD3D11: public Texture
    {
    public:
        TextureD3D11(Renderer* renderer);
        virtual ~TextureD3D11();

        virtual bool initFromFile(const std::string& filename) override;

        ID3D11Texture2D* getTexture() const { return _texture; }
        ID3D11ShaderResourceView* getResourceView() const { return _resourceView; }

    protected:
        ID3D11Texture2D* _texture;
        ID3D11ShaderResourceView* _resourceView;
    };
}
