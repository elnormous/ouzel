// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <MetalKit/MTKView.h>
#include "CompileConfig.h"
#include "RendererMetal.h"
#include "TextureMetal.h"
#include "ShaderMetal.h"
#include "MeshBufferMetal.h"
#ifdef OUZEL_PLATFORM_OSX
    #include "WindowOSX.h"
#elif OUZEL_PLATFORM_TVOS
    #include "WindowTVOS.h"
#elif OUZEL_PLATFORM_IOS
    #include "WindowIOS.h"
#endif
#include "ColorPS.h"
#include "ColorVS.h"
#include "TexturePS.h"
#include "TextureVS.h"
#include "Engine.h"
#include "Cache.h"
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
            if (_commandQueue)
            {
                [_commandQueue release];
                _commandQueue = Nil;
            }

            if (_samplerState)
            {
                [_samplerState release];
                _samplerState = Nil;
            }

            if (_device)
            {
                [_device release];
                _device = Nil;
            }
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
#ifdef OUZEL_PLATFORM_OSX
            std::shared_ptr<WindowOSX> window = std::static_pointer_cast<WindowOSX>(sharedEngine->getWindow());
#elif OUZEL_PLATFORM_TVOS
            std::shared_ptr<WindowTVOS> window = std::static_pointer_cast<WindowTVOS>(sharedEngine->getWindow());
#elif OUZEL_PLATFORM_IOS
            std::shared_ptr<WindowIOS> window = std::static_pointer_cast<WindowIOS>(sharedEngine->getWindow());
#endif
            MTKView* view = window->getNativeView();
            view.device = _device;

            _commandQueue = [_device newCommandQueue];

            if (!_commandQueue)
            {
                log("Failed to create Metal command queue");
                return false;
            }

            /*MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.label = @"MyPipeline";
            pipelineStateDescriptor.sampleCount = view.sampleCount;
            //pipelineStateDescriptor.vertexFunction = vertexProgram;
            //pipelineStateDescriptor.fragmentFunction = fragmentProgram;
            //pipelineStateDescriptor.vertexDescriptor = vertexDescriptor;
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
            pipelineStateDescriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
            pipelineStateDescriptor.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;*/

            MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
            samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
            samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
            samplerDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
            samplerDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;

            _samplerState = [_device newSamplerStateWithDescriptor:samplerDescriptor];
            [samplerDescriptor release];

            if (!_samplerState)
            {
                log("Failed to create Metal sampler state");
                return false;
            }

            ShaderPtr textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_METAL, sizeof(TEXTURE_PIXEL_SHADER_METAL), TEXTURE_VERTEX_SHADER_METAL, sizeof(TEXTURE_VERTEX_SHADER_METAL), VertexPCT::ATTRIBUTES);

            if (!textureShader)
            {
                return false;
            }

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_METAL, sizeof(COLOR_PIXEL_SHADER_METAL), COLOR_VERTEX_SHADER_METAL, sizeof(COLOR_VERTEX_SHADER_METAL), VertexPC::ATTRIBUTES);

            if (!colorShader)
            {
                return false;
            }

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            return true;
        }

        void RendererMetal::setClearColor(Color color)
        {
            /*std::shared_ptr<WindowOSX> windowOSX = std::static_pointer_cast<WindowOSX>(sharedEngine->getWindow());
            MTKView* view = windowOSX->getNativeView();
            MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor;

            //renderPassDescriptor.colorAttachments[0].texture = _texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(color.getR(), color.getG(), color.getB(), color.getA());*/
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
