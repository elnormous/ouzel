// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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
        
        Texture* getTexture() const { return _texture; }
        
    protected:
        RenderTarget();
        
        Texture* _texture = nullptr;
    };
}
