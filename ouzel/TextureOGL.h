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
        TextureOGL(Renderer* renderer);
        virtual ~TextureOGL();
        
        virtual bool loadFromFile(const std::string& filename) override;
        
        GLuint getTextureId() const { return _textureId; }
        
    protected:
        GLuint _textureId = 0;
    };
}
