// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#ifdef OUZEL_PLATFORM_OSX
#include <OpenGL/gl3.h>
#endif

#include "Texture.h"

namespace ouzel
{
    class TextureOGL: public Texture
    {
    public:
        TextureOGL(const std::string& filename, Renderer* renderer);
        virtual ~TextureOGL();
        
        GLuint getTextureId() const { return _textureId; }
        
    protected:
        GLuint _textureId = 0;
    };
}
