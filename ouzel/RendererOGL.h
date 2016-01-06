// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Renderer.h"

#if defined(OUZEL_PLATFORM_OSX)
#include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#endif

namespace ouzel
{    
    class RendererOGL: public Renderer
    {
        friend Engine;
    public:
        virtual ~RendererOGL();
        
        bool initOpenGL(uint32_t width, uint32_t height);
        bool checkOpenGLErrors();
        
        virtual void setClearColor(Color color) override;
        
        virtual void resize(const Size2& size) override;
        
        virtual void clear() override;
        virtual void flush() override;
        
        virtual std::shared_ptr<Texture> loadTextureFromFile(const std::string& filename, bool dynamic) override;
        virtual std::shared_ptr<Texture> loadTextureFromData(const void* data, const Size2& size, bool dynamic) override;
        
        virtual std::shared_ptr<Shader> loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes) override;
        virtual std::shared_ptr<Shader> loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes) override;
        
        virtual std::shared_ptr<MeshBuffer> createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes) override;
        virtual bool drawMeshBuffer(std::shared_ptr<MeshBuffer> meshBuffer) override;
        
        virtual bool drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform = Matrix4()) override;
        virtual bool drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform = Matrix4()) override;
        virtual bool drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform = Matrix4()) override;
        
    private:
        RendererOGL(const Size2& size, bool resiazble, bool fullscreen);
        
        bool _ready = false;
    };
}
