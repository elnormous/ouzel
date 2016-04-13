// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"
#include "RendererMetal.h"
#include "TextureMetal.h"
#include "ShaderMetal.h"
#include "MeshBufferMetal.h"
#include "BlendStateMetal.h"
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
            if (_currentCommandBuffer)
            {
                [_currentCommandBuffer release];
                _currentCommandBuffer = Nil;
            }

            if (_pipelineState)
            {
                [_pipelineState release];
                _pipelineState = Nil;
            }

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
            _view = window->getNativeView();
            _view.device = _device;

            _commandQueue = [_device newCommandQueue];

            if (!_commandQueue)
            {
                log("Failed to create Metal command queue");
                return false;
            }

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

            ShaderPtr textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_METAL, sizeof(TEXTURE_PIXEL_SHADER_METAL), TEXTURE_VERTEX_SHADER_METAL, sizeof(TEXTURE_VERTEX_SHADER_METAL), VertexPCT::ATTRIBUTES, "main_ps", "main_vs");

            if (!textureShader)
            {
                return false;
            }

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_METAL, sizeof(COLOR_PIXEL_SHADER_METAL), COLOR_VERTEX_SHADER_METAL, sizeof(COLOR_VERTEX_SHADER_METAL), VertexPC::ATTRIBUTES, "main_ps", "main_vs");

            if (!colorShader)
            {
                return false;
            }

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStatePtr noBlendState = createBlendState(false,
                                                          BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                          BlendState::BlendOperation::ADD,
                                                          BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                          BlendState::BlendOperation::ADD);

            if (!noBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStatePtr alphaBlendState = createBlendState(true,
                                                             BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                                             BlendState::BlendOperation::ADD,
                                                             BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                             BlendState::BlendOperation::ADD);

            if (!alphaBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            std::shared_ptr<ShaderMetal> textureShaderMetal = std::static_pointer_cast<ShaderMetal>(textureShader);

            MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.sampleCount = _view.sampleCount;
            pipelineStateDescriptor.vertexFunction = textureShaderMetal->getVertexShader();
            pipelineStateDescriptor.fragmentFunction = textureShaderMetal->getPixelShader();
            pipelineStateDescriptor.vertexDescriptor = textureShaderMetal->getVertexDescriptor();
            pipelineStateDescriptor.colorAttachments[0].pixelFormat = _view.colorPixelFormat;
            pipelineStateDescriptor.depthAttachmentPixelFormat = _view.depthStencilPixelFormat;
            pipelineStateDescriptor.stencilAttachmentPixelFormat = _view.depthStencilPixelFormat;

            // blending
            pipelineStateDescriptor.colorAttachments[0].blendingEnabled = alphaBlendState->isBlendingEnabled() ? YES : NO;

            pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = BlendStateMetal::getBlendFactor(alphaBlendState->getColorBlendSource());
            pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = BlendStateMetal::getBlendFactor(alphaBlendState->getColorBlendDest());
            pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = BlendStateMetal::getBlendOperation(alphaBlendState->getColorOperation());

            pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = BlendStateMetal::getBlendFactor(alphaBlendState->getAlphaBlendSource());
            pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = BlendStateMetal::getBlendFactor(alphaBlendState->getAlphaBlendDest());
            pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = BlendStateMetal::getBlendOperation(alphaBlendState->getAlphaOperation());

            pipelineStateDescriptor.colorAttachments[0].writeMask = MTLColorWriteMaskAll;

            NSError* error = Nil;
            _pipelineState = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
            [pipelineStateDescriptor release];
            if (!_pipelineState)
            {
                log("Failed to created Metal pipeline state");
                return false;
            }

            ouzel::sharedEngine->begin();

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
            if (_currentRenderCommandEncoder)
            {
                [_currentRenderCommandEncoder endEncoding];
                _currentRenderCommandEncoder = Nil;
            }

            if (_currentCommandBuffer)
            {
                [_currentCommandBuffer presentDrawable:_view.currentDrawable];
                
                [_currentCommandBuffer commit];
                [_currentCommandBuffer release];
                _currentCommandBuffer = Nil;
            }

            _currentRenderPassDescriptor = Nil;
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

            if (!_activeShader || _activeShader->getVertexAttributes() != VertexPCT::ATTRIBUTES)
            {
                return false;
            }

            if (!_currentCommandBuffer)
            {
                _currentCommandBuffer = [[_commandQueue commandBuffer] retain];

                if (!_currentCommandBuffer)
                {
                    log("Failed to create Metal command buffer");
                    return false;
                }

                _currentRenderPassDescriptor = _view.currentRenderPassDescriptor;

                if (!_currentRenderPassDescriptor)
                {
                    log("Failed to get Metal render pass descriptor");
                    return false;
                }

                _currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                _currentRenderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(_clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA());
            }

            std::shared_ptr<MeshBufferMetal> meshBufferMetal = std::static_pointer_cast<MeshBufferMetal>(meshBuffer);

            if (!_currentRenderCommandEncoder)
            {
                _currentRenderCommandEncoder = [_currentCommandBuffer renderCommandEncoderWithDescriptor:_currentRenderPassDescriptor];
            }

            [_currentRenderCommandEncoder setRenderPipelineState:_pipelineState];
            [_currentRenderCommandEncoder setVertexBuffer:meshBufferMetal->getVertexBuffer() offset:0 atIndex:0];

            if (_activeShader)
            {
                std::shared_ptr<ShaderMetal> shaderMetal = std::static_pointer_cast<ShaderMetal>(_activeShader);
                [_currentRenderCommandEncoder setFragmentBuffer:shaderMetal->getPixelShaderConstantBuffer() offset:0 atIndex:1];
                [_currentRenderCommandEncoder setVertexBuffer:shaderMetal->getVertexShaderConstantBuffer() offset:0 atIndex:1];
            }

            for (uint32_t layer = 0; layer < TEXTURE_LAYERS; ++layer)
            {
                if (_activeTextures[layer])
                {
                    std::shared_ptr<TextureMetal> textureMetal = std::static_pointer_cast<TextureMetal>(_activeTextures[layer]);
                    
                    [_currentRenderCommandEncoder setFragmentTexture:textureMetal->getTexture() atIndex:layer];
                }
                else
                {
                    [_currentRenderCommandEncoder setFragmentTexture:Nil atIndex:layer];
                }
            }

            [_currentRenderCommandEncoder setFragmentSamplerState:_samplerState atIndex:0];

            if (indexCount == 0)
            {
                indexCount = meshBufferMetal->getIndexCount();
            }

            MTLPrimitiveType primitiveType;

            switch (drawMode)
            {
                case DrawMode::POINT_LIST: primitiveType = MTLPrimitiveTypePoint; break;
                case DrawMode::LINE_LIST: primitiveType = MTLPrimitiveTypeLine; break;
                case DrawMode::LINE_STRIP: primitiveType = MTLPrimitiveTypeLineStrip; break;
                case DrawMode::TRIANGLE_LIST: primitiveType = MTLPrimitiveTypeTriangle; break;
                case DrawMode::TRIANGLE_STRIP: primitiveType = MTLPrimitiveTypeTriangleStrip; break;
            }

            [_currentRenderCommandEncoder drawIndexedPrimitives:primitiveType
                                                     indexCount:indexCount
                                                      indexType:meshBufferMetal->getIndexFormat()
                                                    indexBuffer:meshBufferMetal->getIndexBuffer()
                                              indexBufferOffset:0];

            return true;
        }
    } // namespace video
} // namespace ouzel
