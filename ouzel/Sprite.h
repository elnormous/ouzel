// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Node.h"

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
        
        float getWidth() const { return _width; }
        float getHeight() const { return _height; }
        
        virtual bool checkVisibility() const override;
        
    protected:
        Texture* _texture;
        Shader* _shader;
        
        Engine* _engine;
        
        float _width;
        float _height;
        
        GLuint _vertexArray;
        GLuint _indexBuffer;
        GLuint _vertexBuffer;
        GLuint _colorBuffer;
        GLuint _texcoordBuffer;
    };
}
