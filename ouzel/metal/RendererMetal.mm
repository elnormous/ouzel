// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <MetalKit/MTKView.h>
#include "RendererMetal.h"
#include "TextureMetal.h"
#include "ShaderMetal.h"
#include "MeshBufferMetal.h"
#include "WindowOSX.h"
#include "Engine.h"
#include "Utils.h"

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

            if (!_device)
            {
                log("Failed to create Metal device");
                return false;
            }

            std::shared_ptr<WindowOSX> windowOSX = std::static_pointer_cast<WindowOSX>(sharedEngine->getWindow());
            MTKView* view = windowOSX->getNativeView();
            view.device = _device;

            _commandQueue = [_device newCommandQueue];

            if (!_commandQueue)
            {
                log("Failed to create command queue");
                return false;
            }

            MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.label = @"MyPipeline";
            pipelineStateDescriptor.sampleCount = view.sampleCount;
            //pipelineStateDescriptor.vertexFunction = vertexProgram;
            //pipelineStateDescriptor.fragmentFunction = fragmentProgram;
            //pipelineStateDescriptor.vertexDescriptor = vertexDescriptor;
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
            pipelineStateDescriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
            pipelineStateDescriptor.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;

            return true;
        }

        void RendererMetal::setClearColor(Color color)
        {
            std::shared_ptr<WindowOSX> windowOSX = std::static_pointer_cast<WindowOSX>(sharedEngine->getWindow());
            MTKView* view = windowOSX->getNativeView();
            MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor;

            //renderPassDescriptor.colorAttachments[0].texture = _texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(color.getR(), color.getG(), color.getB(), color.getA());
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

        TexturePtr RendererMetal::loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureMetal> texture(new TextureMetal());

            if (!texture->initFromFile(filename, dynamic, mipmaps))
            {
                texture.reset();
            }

            return texture;
        }

        TexturePtr RendererMetal::loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            std::shared_ptr<TextureMetal> texture(new TextureMetal());

            if (!texture->initFromData(data, size, dynamic, mipmaps))
            {
                texture.reset();
            }

            return texture;
        }

        ShaderPtr RendererMetal::loadShaderFromFiles(const std::string& fragmentShader,
                                                     const std::string& vertexShader,
                                                     uint32_t vertexAttributes,
                                                     const std::string& fragmentShaderFunction,
                                                     const std::string& vertexShaderFunction)
        {
            std::shared_ptr<ShaderMetal> shader(new ShaderMetal());

            if (!shader->initFromFiles(fragmentShader, vertexShader, vertexAttributes, fragmentShaderFunction, vertexShaderFunction))
            {
                shader.reset();
            }

            return shader;
        }

        ShaderPtr RendererMetal::loadShaderFromBuffers(const uint8_t* fragmentShader,
                                                       uint32_t fragmentShaderSize,
                                                       const uint8_t* vertexShader,
                                                       uint32_t vertexShaderSize,
                                                       uint32_t vertexAttributes,
                                                       const std::string& fragmentShaderFunction,
                                                       const std::string& vertexShaderFunction)
        {
            std::shared_ptr<ShaderMetal> shader(new ShaderMetal());

            if (!shader->initFromBuffers(fragmentShader, fragmentShaderSize, vertexShader, vertexShaderSize, vertexAttributes, fragmentShaderFunction, vertexShaderFunction))
            {
                shader.reset();
            }

            return shader;
        }

        MeshBufferPtr RendererMetal::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferMetal> meshBuffer(new MeshBufferMetal());

            if (!meshBuffer->init())
            {
                meshBuffer.reset();
            }

            return meshBuffer;
        }

        MeshBufferPtr RendererMetal::createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
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
