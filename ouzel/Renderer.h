// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "AutoPtr.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Rectangle.h"
#include "Matrix4.h"
#include "Size2.h"
#include "Color.h"
#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"

namespace ouzel
{
    const std::string SHADER_TEXTURE = "shaderTexture";
    const std::string SHADER_COLOR = "shaderColor";
    
    class Engine;
    class Node;
    class Sprite;
    class MeshBuffer;

    class Renderer: public Noncopyable, public ReferenceCounted
    {
        friend Engine;
    public:
        static const uint32_t TEXTURE_LAYERS = 2;
    
        enum class Driver
        {
            NONE = 0,
            OPENGL,
            DIRECT3D11
        };

        static Renderer* getInstance();

        virtual ~Renderer();
        
        Driver getDriver() const { return _driver; }
        
        virtual void setClearColor(Color color) { _clearColor = color; }
        virtual Color getClearColor() const { return _clearColor; }
        
        virtual void recalculateProjection();
        
        virtual void begin();
        virtual void clear();
        virtual void flush();
        
        virtual const Size2& getSize() const { return _size; }
        virtual void resize(const Size2& size);
        
        virtual bool getResizable() const { return _resizable; }
        
        virtual const std::string& getTitle() const { return _title; }
        virtual void setTitle(const std::string& title) { _title = title; }
        
        void preloadTexture(const std::string& filename);
        Texture* getTexture(const std::string& filename);
        virtual Texture* loadTextureFromFile(const std::string& filename);
        virtual bool activateTexture(Texture* texture, uint32_t layer);
        virtual Texture* getActiveTexture(uint32_t layer) const { return _activeTextures[layer]; }
        
        Shader* getShader(const std::string& shaderName) const;
        void setShader(const std::string& shaderName, Shader* shader);
        virtual Shader* loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes);
        virtual Shader* loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes);
        virtual bool activateShader(Shader* shader);
        virtual Shader* getActiveShader() const { return _activeShader; }
        
        virtual MeshBuffer* createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes);
        virtual bool drawMeshBuffer(MeshBuffer* meshBuffer);
        
        const Matrix4& getProjection() const { return _projection; }
        
        Vector2 absoluteToWorldLocation(const Vector2& position);
        Vector2 worldToAbsoluteLocation(const Vector2& position);
        
        virtual bool drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform = Matrix4());
        virtual bool drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform = Matrix4());
        virtual bool drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform = Matrix4());
        
    protected:
        Renderer(const Size2& size, bool resizable, bool fullscreen, Driver driver = Driver::NONE);
        
        Driver _driver;
        
        Color _clearColor;
        
        Matrix4 _projection;
        
        std::unordered_map<std::string, AutoPtr<Texture>> _textures;
        std::unordered_map<std::string, AutoPtr<Shader>> _shaders;
        
        AutoPtr<Texture> _activeTextures[TEXTURE_LAYERS];
        AutoPtr<Shader> _activeShader = nullptr;
        
        Size2 _size;
        bool _resizable = false;
        bool _fullscreen = false;
        
        std::string _title = "Ouzel";
    };
}
