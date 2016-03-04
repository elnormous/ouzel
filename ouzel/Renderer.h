// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "Types.h"
#include "Noncopyable.h"
#include "Rectangle.h"
#include "Matrix4.h"
#include "Size2.h"
#include "Color.h"
#include "AABB2.h"
#include "Vertex.h"
#include "Shader.h"

namespace ouzel
{
    const std::string SHADER_TEXTURE = "shaderTexture";
    const std::string SHADER_COLOR = "shaderColor";
    
    class Engine;
    class Node;
    class Sprite;
    class MeshBuffer;
    class Window;

    class Renderer: public Noncopyable
    {
        friend Engine;
        friend Window;
    public:
        static const uint32_t TEXTURE_LAYERS = 2;
    
        enum class Driver
        {
            NONE = 0,
            OPENGL,
            DIRECT3D11
        };
        
        enum class DrawMode
        {
            POINT_LIST = 0,
            LINE_LIST,
            LINE_STRIP,
            TRIANGLE_LIST,
            TRIANGLE_STRIP
        };

        virtual ~Renderer();
        
        Driver getDriver() const { return _driver; }
        
        virtual void setClearColor(Color color) { _clearColor = color; }
        virtual Color getClearColor() const { return _clearColor; }
        
        virtual void clear();
        virtual void present();
        virtual void flush();
        
        const Size2& getSize() const { return _size; }

        virtual std::vector<Size2> getSupportedResolutions() const;

        virtual TexturePtr createTexture(const Size2& size, bool dynamic, bool mipmaps = true);
        virtual TexturePtr loadTextureFromFile(const std::string& filename, bool dynamic = false, bool mipmaps = true);
        virtual TexturePtr loadTextureFromData(const void* data, const Size2& size, bool dynamic = false, bool mipmaps = true);
        virtual bool activateTexture(const TexturePtr& texture, uint32_t layer);
        virtual TexturePtr getActiveTexture(uint32_t layer) const { return _activeTextures[layer]; }
        virtual RenderTargetPtr createRenderTarget(const Size2& size, bool depthBuffer);
        virtual bool activateRenderTarget(const RenderTargetPtr& renderTarget);
        
        virtual ShaderPtr loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes);
        virtual ShaderPtr loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes);
        virtual bool activateShader(const ShaderPtr& shader);
        virtual ShaderPtr getActiveShader() const { return _activeShader; }
        
        virtual MeshBufferPtr createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes);
        virtual bool drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST);
        
        Vector2 viewToScreenLocation(const Vector2& position);
        Vector2 screenToViewLocation(const Vector2& position);
        bool checkVisibility(const Matrix4& transform, const AABB2& boundingBox, const CameraPtr& camera);
        
        virtual bool saveScreenshot(const std::string& filename);
        
        virtual uint32_t getDrawCallCount() const { return _drawCallCount; }
        
    protected:
        Renderer(Driver driver = Driver::NONE);
        virtual bool init(const Size2& size, bool fullscreen);
        
        virtual void setSize(const Size2& size);
        virtual void setFullscreen(bool fullscreen);
        
        Driver _driver;
        Size2 _size;
        bool _fullscreen = false;
        
        Color _clearColor;
        
        TexturePtr _activeTextures[TEXTURE_LAYERS];
        ShaderPtr _activeShader;
        RenderTargetPtr _activeRenderTarget;
        
        uint32_t _drawCallCount = 0;
    };
}
