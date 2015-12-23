// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX)
#include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

#endif

#include "Texture.h"

namespace ouzel
{
    class TextureOGL: public Texture
    {
    public:
        TextureOGL(Renderer* renderer);
        virtual ~TextureOGL();
        
        virtual bool initFromFile(const std::string& filename) override;
        
        GLuint getTextureId() const { return _textureId; }
        
    protected:
        GLuint _textureId = 0;
    };
}
