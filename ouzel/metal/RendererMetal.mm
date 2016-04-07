// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererMetal.h"
#include "TextureMetal.h"
#include "ShaderMetal.h"
#include "MeshBufferMetal.h"

namespace ouzel
{
    namespace video
    {
        RendererMetal::RendererMetal()
        {
            
        }
        
        RendererMetal::~RendererMetal()
        {
            destroy();
        }
        
        void RendererMetal::destroy()
        {
            if (_commandQueue) [_commandQueue release];
            if (_device) [_device release];
        }
        
        bool RendererMetal::init(const Size2& size, bool fullscreen)
        {
            destroy();
            
            _device = MTLCreateSystemDefaultDevice();
            
            _commandQueue = [_device newCommandQueue];
            
            return true;
        }
        
        void RendererMetal::setClearColor(Color color)
        {

        }

        void RendererMetal::setSize(const Size2& size)
        {

        }
        
        void RendererMetal::clear()
        {

        }
        
        void RendererMetal::flush()
        {

        }
        
        std::shared_ptr<Texture> RendererMetal::loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureMetal> texture(new TextureMetal());
            
            if (!texture->initFromFile(filename, dynamic, mipmaps))
            {
                texture.reset();
            }
            
            return texture;
        }
        
        std::shared_ptr<Texture> RendererMetal::loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureMetal> texture(new TextureMetal());
            
            if (!texture->initFromData(data, size, dynamic, mipmaps))
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
        
        std::shared_ptr<MeshBuffer> RendererMetal::createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
        {
            std::shared_ptr<MeshBufferMetal> meshBuffer(new MeshBufferMetal());
            
            if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexAttributes, vertexCount, dynamicVertexBuffer))
            {
                meshBuffer.reset();
            }
            
            return meshBuffer;
        }
        
        bool RendererMetal::drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount, DrawMode drawMode)
        {
            if (!Renderer::drawMeshBuffer(meshBuffer, indexCount, drawMode))
            {
                return false;
            }
            
            return true;
        }
    } // namespace video
} // namespace ouzel
