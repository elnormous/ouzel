// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetD3D11.h"
#include "TextureD3D11.h"

namespace ouzel
{
    RenderTargetD3D11::RenderTargetD3D11()
    {
        
    }
    
    RenderTargetD3D11::~RenderTargetD3D11()
    {
        
    }

    bool RenderTargetD3D11::init(Size2 const& size, bool depthBuffer)
    {
        if (!RenderTarget::init(size, depthBuffer))
        {
            return false;
        }

        std::shared_ptr<TextureD3D11> textureD3D11(new TextureD3D11());

        if (!textureD3D11->init(size, false))
        {
            return false;
        }

        _texture = textureD3D11;

        return true;
    }
}
