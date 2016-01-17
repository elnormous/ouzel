// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Node.h"
#include "Size2.h"
#include "MeshBuffer.h"
#include "Rectangle.h"

namespace ouzel
{
    class Engine;
    class SceneManager;
    class Texture;
    class Shader;
    
    class Sprite: public Node
    {
    public:
        Sprite();
        virtual ~Sprite();
        
        virtual bool initFromFile(const std::string& filename);
        
        virtual void update(float delta) override;
        virtual void draw() override;
        
        virtual void setOpacity(float opacity) override;
        
        virtual TexturePtr getTexture() const { return _texture; }
        virtual void setTexture(TexturePtr const& texture);
        
        virtual ShaderPtr getShader() const { return _shader; }
        virtual void setShader(ShaderPtr const& shader);
        
        virtual const Size2& getSize() const { return _size; }
        
        virtual const Color& getColor() const { return _color; }
        virtual void setColor(const Color& color);
        
        virtual bool checkVisibility() const override;
        
        virtual void play(bool repeat = true, float frameInterval = 0.1f);
        virtual void stop(bool reset = true);
        virtual bool isPlaying() const { return _playing; }
        
    protected:
        bool loadSpriteSheet(const std::string& filename);
        void addFrame(const Rectangle& rectangle, const Size2& textureSize, bool rotated, const Size2& sourceSize, const Vector2& offset, const Vector2& pivot);
        
        void updateVertexColor();
        
        TexturePtr _texture;
        ShaderPtr _shader;
        
        Size2 _size;
        
        std::vector<std::vector<VertexPCT>> _frameVertices;
        std::vector<MeshBufferPtr> _frameMeshBuffers;
        Color _color = Color(255, 255, 255, 255);
        
        uint32_t _currentFrame = 0;
        float _frameInterval = 0.0f;
        bool _playing = false;
        bool _repeat = false;
        float _timeSinceLastFrame = 0.0f;
        
        uint32_t _uniModelViewProj;
    };
}
