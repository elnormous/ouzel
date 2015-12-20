// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

#ifdef OUZEL_PLATFORM_OSX
#include <OpenGL/gl3.h>
#endif

#include "Node.h"
#include "Size2.h"

namespace ouzel
{
    class Engine;
    class Scene;
    class Texture;
    class Shader;
    
    class Sprite: public Node
    {
    public:
        Sprite(const std::string& filename, Scene* scene);
        virtual ~Sprite();
        
        virtual void draw() override;
        
        Texture* getTexture() const { return _texture; }
        
        const Size2& getSize() const { return _size; }
        
        virtual bool checkVisibility() const override;
        
    protected:
        Texture* _texture;
        Shader* _shader;
        
        Engine* _engine;
        
        Size2 _size;
        
        GLuint _vertexArray;
        GLuint _indexBuffer;
        GLuint _vertexBuffer;
        GLuint _colorBuffer;
        GLuint _texcoordBuffer;
    };
}
