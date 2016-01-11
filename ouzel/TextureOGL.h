// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
#include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

#include "Texture.h"

namespace ouzel
{
    class RendererOGL;
    
    class TextureOGL: public Texture
    {
        friend RendererOGL;
    public:
        virtual ~TextureOGL();
        
        virtual bool initFromData(const void* data, const Size2& size, bool dynamic) override;
        
        virtual bool upload(const void* data, const Size2& size) override;
        
        GLuint getTextureId() const { return _textureId; }
        
    protected:
        TextureOGL();
        
        void clean();
        
        bool uploadData(const void* data, GLsizei width, GLsizei height);
        
        GLuint _textureId = 0;
    };
}
