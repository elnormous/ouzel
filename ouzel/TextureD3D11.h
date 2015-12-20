// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Renderer.h"

namespace ouzel
{
    class TextureD3D11: public Renderer
    {
    public:
        TextureD3D11(Engine* engine);
        
        virtual void activateTexture(Texture* texture, uint32_t layer);
    };
}
