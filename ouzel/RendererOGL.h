// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Renderer.h"

namespace ouzel
{
    class RendererOGL: public Renderer
    {
    public:
        RendererOGL(Engine* engine);
        
        virtual void activateTexture(Texture* texture, uint32_t layer);
        virtual void activateShader(Shader* shader);
    };
}
