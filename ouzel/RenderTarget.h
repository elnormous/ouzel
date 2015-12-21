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
    public:
        RenderTarget(Renderer* renderer);
        virtual ~RenderTarget();
        
        Texture* getTexture() const { return _texture; }
        
    protected:
        Renderer* _renderer;
        Texture* _texture = nullptr;
    };
}
