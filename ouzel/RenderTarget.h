// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"

namespace ouzel
{
    class Texture;
    
    class RenderTarget: public Noncopyable
    {
    public:
        RenderTarget();
        virtual ~RenderTarget();
        
        Texture* getTexture() const { return _texture; }
        
    protected:
        Texture* _texture = nullptr;
    };
}
