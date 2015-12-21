// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <unordered_map>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Rectangle.h"
#include "Matrix4.h"
#include "Size2.h"
#include "Color.h"

namespace ouzel
{
    const std::string SHADER_TEXTURE = "shaderTexture";
    const std::string SHADER_COLOR = "shaderColor";
    
    class Engine;
    class View;
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
        
        Renderer(const Size2& size, bool fullscreen, Engine* engine, Driver driver = Driver::NONE);
        virtual ~Renderer();
        
        Engine* getEngine() const { return _engine; }
        Driver getDriver() const { return _driver; }
        View* getView() const { return _view; }
        
        void setClearColor(Color color) { _clearColor = color; }
        Color getClearColor() const { return _clearColor; }
        
        virtual void recalculateProjection();
        
        virtual void begin();
        virtual void clear();
        virtual void flush();
        
        void preloadTexture(const std::string& filename);
        Texture* getTexture(const std::string& filename);
        virtual Texture* loadTextureFromFile(const std::string& filename);
        virtual void activateTexture(Texture* texture, uint32_t layer);
        
        Shader* getShader(const std::string& shaderName) const;
        void setShader(const std::string& shaderName, Shader* shader);
        virtual Shader* loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader);
        virtual Shader* loadShaderFromStrings(const std::string& fragmentShader, const std::string& vertexShader);
        virtual void activateShader(Shader* shader);
        
        const Matrix4& getProjection() const { return _projection; }
        
        Vector2 absoluteToWorldLocation(const Vector2& position);
        Vector2 worldToAbsoluteLocation(const Vector2& position);
        
        virtual void drawLine(const Vector2& start, const Vector2& finish, const Vector3& color, const Matrix4& transform = Matrix4());
        virtual void drawRectangle(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform = Matrix4());
        virtual void drawQuad(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform = Matrix4());
        
    protected:
        Engine* _engine;
        Driver _driver;
        View* _view = nullptr;
        
        Color _clearColor;
        
        Matrix4 _projection;
        
        std::unordered_map<std::string, Texture*> _textures;
        
        std::unordered_map<std::string, Shader*> _shaders;
    };
}
