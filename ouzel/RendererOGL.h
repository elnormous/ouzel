//
//  Renderer.h
//  Ouzel
//
//  Created by Elviss Strazdins on 10/03/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

#pragma once

#include "Renderer.h"

namespace ouzel
{
    class RendererOGL: public Renderer
    {
    public:
        RendererOGL(Engine* engine);
        
        virtual void activateTexture(Texture* texture, uint32_t layer);
    };
}
