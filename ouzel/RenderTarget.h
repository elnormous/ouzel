// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <algorithm>
#include <memory>
#include "Types.h"
#include "Noncopyable.h"

namespace ouzel
{
    class Renderer;
    class Texture;
    
    class RenderTarget: public Noncopyable
    {
        friend Renderer;
    public:
        virtual ~RenderTarget();
        
        TexturePtr getTexture() const { return _texture; }
        
    protected:
        RenderTarget();
        
        TexturePtr _texture = nullptr;
    };
}
