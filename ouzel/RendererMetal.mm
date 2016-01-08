// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererMetal.h"

namespace ouzel
{
    RendererMetal::RendererMetal()
    {
        
    }
    
    RendererMetal::~RendererMetal()
    {
        
    }
    
    void RendererMetal::setClearColor(Color color)
    {

    }

    void RendererMetal::resize(const Size2& size)
    {

    }
    
    void RendererMetal::clear()
    {

    }
    
    void RendererMetal::flush()
    {

    }
    
    std::shared_ptr<Texture> RendererMetal::loadTextureFromFile(const std::string& filename, bool dynamic)
    {
        std::shared_ptr<TextureMetal> texture(new TextureMetal());
        
        if (!texture->initFromFile(filename, dynamic))
        {
            texture.reset();
        }
        
        return texture;
    }
    
    std::shared_ptr<Texture> RendererMetal::loadTextureFromData(const void* data, const Size2& size, bool dynamic)
    {
        std::shared_ptr<TextureMetal> texture(new TextureMetal());
        
        if (!texture->initFromData(data, size, dynamic))
        {
            texture.reset();
        }
        
        return texture;
    }
    
    std::shared_ptr<Shader> RendererMetal::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes)
    {
        std::shared_ptr<ShaderMetal> shader(new ShaderMetal());
        
        if (!shader->initFromFiles(fragmentShader, vertexShader, vertexAttributes))
        {
            shader.reset();
        }
        
        return shader;
    }
    
    std::shared_ptr<Shader> RendererMetal::loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes)
    {
        std::shared_ptr<ShaderMetal> shader(new ShaderMetal());
        
        if (!shader->initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes))
        {
            shader.reset();
        }
        
        return shader;
    }
    
    std::shared_ptr<MeshBuffer> RendererMetal::createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes)
    {
        std::shared_ptr<MeshBufferMetal> meshBuffer(new MeshBufferMetal());
        
        if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexSize, vertexCount, dynamicVertexBuffer, vertexAttributes))
        {
            meshBuffer.reset();
        }
        
        return meshBuffer;
    }
    
    bool RendererMetal::drawMeshBuffer(std::shared_ptr<MeshBuffer> const& meshBuffer)
    {
        if (!Renderer::drawMeshBuffer(meshBuffer))
        {
            return false;
        }
        
        return true;
    }
    
    bool RendererMetal::drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform)
    {
        return true;
    }
    
    bool RendererMetal::drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
        return true;
    }
    
    bool RendererMetal::drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
        return true;
    }
}
