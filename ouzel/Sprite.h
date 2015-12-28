// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "AutoPtr.h"
#include "Node.h"
#include "Size2.h"
#include "MeshBuffer.h"

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
        
        virtual Texture* getTexture() const { return _texture; }
        virtual void setTexture(Texture* texture);
        
        virtual Shader* getShader() const { return _shader; }
        virtual void setShader(Shader* shader);
        
        virtual const Size2& getSize() const { return _size; }
        
        virtual const Color& getColor() const { return _color; }
        virtual void setColor(const Color& color);
        
        virtual bool checkVisibility() const override;
        
    protected:
        AutoPtr<Texture> _texture;
        AutoPtr<Shader> _shader;
        
        Engine* _engine;
        
        Size2 _size;
        
        std::vector<Vertex> _vertices;
        AutoPtr<MeshBuffer> _meshBuffer;
        Color _color = Color(255, 255, 255, 255);
        
        uint32_t _uniModelViewProj;
    };
}
