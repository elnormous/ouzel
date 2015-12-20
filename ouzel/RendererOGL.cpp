// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGL.h"
#include "TextureOGL.h"
#include "RenderTargetOGL.h"

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
        glBindTexture(GL_TEXTURE_2D, textureOGL->getTextureId());
    }
}
