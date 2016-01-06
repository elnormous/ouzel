// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <algorithm>
#include <memory>
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
        
        std::shared_ptr<Texture> getTexture() const { return _texture; }
        
    protected:
        RenderTarget();
        
        std::shared_ptr<Texture> _texture = nullptr;
    };
}
