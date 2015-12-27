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
        
        Texture* getTexture() const { return _texture; }
        void setTexture(Texture* texture);
        
        Shader* getShader() const { return _shader; }
        void setShader(Shader* shader);
        
        const Size2& getSize() const { return _size; }
        
        virtual bool checkVisibility() const override;
        
    protected:
        AutoPtr<Texture> _texture;
        AutoPtr<Shader> _shader;
        
        Engine* _engine;
        
        Size2 _size;
        
        AutoPtr<MeshBuffer> _meshBuffer;
        
        uint32_t _uniModelViewProj;
    };
}
