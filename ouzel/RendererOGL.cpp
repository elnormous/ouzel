//
//  Renderer.h
//  Ouzel
//
//  Created by Elviss Strazdins on 10/03/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

#include "RendererOGL.h"
#include "TextureOGL.h"
#include "RenderTextureOGL.h"

namespace ouzel
{
    RendererOGL::RendererOGL(Engine* engine):
        Renderer(engine)
    {
        
    }
    
    void RendererOGL::activateTexture(Texture* texture, uint32_t layer)
    {
        TextureOGL* textureOGL = static_cast<TextureOGL*>(texture);
        
        glActiveTexture(GL_TEXTURE0 + layer);
        glBindTexture(GL_TEXTURE_2D, textureOGL->getTexture());
    }
}
