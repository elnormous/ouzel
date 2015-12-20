// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Rectangle.h"
#include "Matrix4.h"

namespace ouzel
{
    const std::string SHADER_TEXTURE = "shaderTexture";
    const std::string SHADER_COLOR = "shaderColor";
    
    class Engine;
    class Texture;
    class Node;
    class Shader;
    class Sprite;

    class Renderer: public Noncopyable, public ReferenceCounted
    {
    public:
        enum class Driver
        {
            NONE = 0,
            OPENGL,
            DIRECT3D11
        };
        
        Renderer(Engine* engine);
        virtual ~Renderer();
        
        bool initOpenGL(uint32_t width, uint32_t height);
        
        void resize(uint32_t width, uint32_t height);
        
        void clear();
        void flush();
        
        void preloadTexture(const std::string& filename);
        Texture* getTexture(const std::string& filename);
        
        virtual void activateTexture(Texture* texture, uint32_t layer) = 0;
        
        Shader* getShader(const std::string& shaderName) const;
        void setShader(const std::string& shaderName, Shader* shader);
        
        const Matrix4& getProjection() const { return _projection; }
        
        Vector2 absoluteToWorldLocation(const Vector2& position);
        Vector2 worldToAbsoluteLocation(const Vector2& position);
        
        void drawLine(const Vector2& start, const Vector2& finish, const Vector3& color, const Matrix4& transform = Matrix4());
        void drawRectangle(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform = Matrix4());
        void drawQuad(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform = Matrix4());
        
    protected:
        Engine* _engine;
        
        Matrix4 _projection;
        
        uint32_t _width;
        uint32_t _height;
        
        std::unordered_map<std::string, Texture*> _textures;
        
        std::unordered_map<std::string, Shader*> _shaders;
    };
}
