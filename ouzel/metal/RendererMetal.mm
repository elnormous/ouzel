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
    #include "ColorPSOSX.h"
    #include "ColorVSOSX.h"
    #include "TexturePSOSX.h"
    #include "TextureVSOSX.h"
#elif OUZEL_PLATFORM_TVOS
    #include "WindowTVOS.h"
    #include "ColorPSTVOS.h"
    #include "ColorVSTVOS.h"
    #include "TexturePSTVOS.h"
    #include "TextureVSTVOS.h"
#elif OUZEL_PLATFORM_IOS
    #include "WindowIOS.h"
    #include "ColorPSIOS.h"
    #include "ColorVSIOS.h"
    #include "TexturePSIOS.h"
    #include "TextureVSIOS.h"
#endif
#include "Engine.h"
#include "Cache.h"
#include "Utils.h"

namespace ouzel
{
    namespace video
    {
        bool RendererMetal::available()
        {
            id<MTLDevice> device = MTLCreateSystemDefaultDevice();

            if (device)
            {
                [device release];
                return true;
            }

            return false;
        }

        RendererMetal::RendererMetal():
            Renderer(Driver::METAL)
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

            if (_currentRenderCommandEncoder)
            {
                [_currentRenderCommandEncoder release];
                _currentRenderCommandEncoder = Nil;
            }

            for (auto pipelineState : _pipelineStates)
            {
                [pipelineState.second release];
            }

            _pipelineStates.clear();

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

            _inflightSemaphore = dispatch_semaphore_create(3);

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
            //_view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

            _commandQueue = [_device newCommandQueue];

            if (!_commandQueue)
            {
                log("Failed to create Metal command queue");
                return false;
            }

            MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
            samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
            samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
            samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
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

            textureShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_METAL, sizeof(COLOR_PIXEL_SHADER_METAL), COLOR_VERTEX_SHADER_METAL, sizeof(COLOR_VERTEX_SHADER_METAL), VertexPC::ATTRIBUTES, "main_ps", "main_vs");

            if (!colorShader)
            {
                return false;
            }

            colorShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}}, 256);

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

            for (ShaderPtr shader : {textureShader, colorShader})
            {
                std::shared_ptr<ShaderMetal> shaderMetal = std::static_pointer_cast<ShaderMetal>(shader);

                for (BlendStatePtr blendState : {noBlendState, alphaBlendState})
                {
                    std::shared_ptr<BlendStateMetal> blendStateMetal = std::static_pointer_cast<BlendStateMetal>(blendState);

                    if (!createPipelineState(blendStateMetal, shaderMetal))
                    {
                        return false;
                    }
                }

                if (!createPipelineState(nullptr, shaderMetal))
                {
                    return false;
                }
            }

            setSize(size);

            return true;
        }

        void RendererMetal::setClearColor(Color color)
        {
            Renderer::setClearColor(color);
        }

        void RendererMetal::setSize(const Size2& size)
        {
            Renderer::setSize(size);
        }

        void RendererMetal::clear()
        {
            dispatch_semaphore_wait(_inflightSemaphore, DISPATCH_TIME_FOREVER);

            if (_currentCommandBuffer) [_currentCommandBuffer release];

            _currentCommandBuffer = [[_commandQueue commandBuffer] retain];

            if (!_currentCommandBuffer)
            {
                log("Failed to create Metal command buffer");
                return;
            }

            __block dispatch_semaphore_t blockSemaphore = _inflightSemaphore;
            [_currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
             {
                 dispatch_semaphore_signal(blockSemaphore);
             }];

            MTLRenderPassDescriptor* renderPassDescriptor = _view.currentRenderPassDescriptor;

            if (!renderPassDescriptor)
            {
                log("Failed to get Metal render pass descriptor");
                return;
            }

            if (_currentRenderCommandEncoder) [_currentRenderCommandEncoder release];

            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(_clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA());

            _currentRenderCommandEncoder = [[_currentCommandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor] retain];

            if (!_currentRenderCommandEncoder)
            {
                log("Failed to create Metal render command encoder");
                return;
            }
        }

        void RendererMetal::present()
        {
            Renderer::present();

            if (_currentRenderCommandEncoder)
            {
                [_currentRenderCommandEncoder endEncoding];
                [_currentRenderCommandEncoder release];
                _currentRenderCommandEncoder = Nil;
            }

            if (_currentCommandBuffer)
            {
                [_currentCommandBuffer presentDrawable:_view.currentDrawable];
                
                [_currentCommandBuffer commit];
                [_currentCommandBuffer release];
                _currentCommandBuffer = Nil;
            }
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

        ShaderPtr RendererMetal::loadShaderFromFiles(const std::string& pixelShader,
                                                     const std::string& vertexShader,
                                                     uint32_t vertexAttributes,
                                                     const std::string& pixelShaderFunction,
                                                     const std::string& vertexShaderFunction)
        {
            std::shared_ptr<ShaderMetal> shader(new ShaderMetal());

            if (!shader->initFromFiles(pixelShader, vertexShader, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                shader.reset();
            }

            return shader;
        }

        ShaderPtr RendererMetal::loadShaderFromBuffers(const uint8_t* pixelShader,
                                                       uint32_t pixelShaderSize,
                                                       const uint8_t* vertexShader,
                                                       uint32_t vertexShaderSize,
                                                       uint32_t vertexAttributes,
                                                       const std::string& pixelShaderFunction,
                                                       const std::string& vertexShaderFunction)
        {
            std::shared_ptr<ShaderMetal> shader(new ShaderMetal());

            if (!shader->initFromBuffers(pixelShader, pixelShaderSize, vertexShader, vertexShaderSize, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
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

            if (!_activeShader)
            {
                return false;
            }

            std::shared_ptr<MeshBufferMetal> meshBufferMetal = std::static_pointer_cast<MeshBufferMetal>(meshBuffer);

            [_currentRenderCommandEncoder setVertexBuffer:meshBufferMetal->getVertexBuffer() offset:0 atIndex:0];

            std::shared_ptr<ShaderMetal> shaderMetal = std::static_pointer_cast<ShaderMetal>(_activeShader);
            [_currentRenderCommandEncoder setFragmentBuffer:shaderMetal->getPixelShaderConstantBuffer()
                                                     offset:shaderMetal->getPixelShaderConstantBufferOffset()
                                                    atIndex:1];
            [_currentRenderCommandEncoder setVertexBuffer:shaderMetal->getVertexShaderConstantBuffer()
                                                   offset:shaderMetal->getVertexShaderConstantBufferOffset()
                                                  atIndex:1];

            std::shared_ptr<BlendStateMetal> blendStateMetal = std::static_pointer_cast<BlendStateMetal>(_activeBlendState);

            auto pipelineStateIterator = _pipelineStates.find(std::make_pair(blendStateMetal, shaderMetal));

            if (pipelineStateIterator != _pipelineStates.end())
            {
                [_currentRenderCommandEncoder setRenderPipelineState:pipelineStateIterator->second];
            }
            else
            {
                id<MTLRenderPipelineState> pipelineState = createPipelineState(blendStateMetal, shaderMetal);

                if (!pipelineState)
                {
                    return false;
                }

                [_currentRenderCommandEncoder setRenderPipelineState:pipelineState];
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

            shaderMetal->nextBuffers();

            return true;
        }

        MTLRenderPipelineStatePtr RendererMetal::createPipelineState(const std::shared_ptr<BlendStateMetal>& blendState,
                                                                     const std::shared_ptr<ShaderMetal>& shader)
        {
            MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.sampleCount = _view.sampleCount;
            pipelineStateDescriptor.vertexFunction = shader->getVertexShader();
            pipelineStateDescriptor.fragmentFunction = shader->getPixelShader();
            pipelineStateDescriptor.vertexDescriptor = shader->getVertexDescriptor();
            pipelineStateDescriptor.depthAttachmentPixelFormat = _view.depthStencilPixelFormat;
            pipelineStateDescriptor.stencilAttachmentPixelFormat = _view.depthStencilPixelFormat;

            pipelineStateDescriptor.colorAttachments[0].pixelFormat = _view.colorPixelFormat;

            // blending
            if (!blendState)
            {
                pipelineStateDescriptor.colorAttachments[0].blendingEnabled = NO;

                pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorZero;
                pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;

                pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
                pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorZero;
                pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
            }
            else
            {
                pipelineStateDescriptor.colorAttachments[0].blendingEnabled = blendState->isBlendingEnabled() ? YES : NO;

                pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = BlendStateMetal::getBlendFactor(blendState->getColorBlendSource());
                pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = BlendStateMetal::getBlendFactor(blendState->getColorBlendDest());
                pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = BlendStateMetal::getBlendOperation(blendState->getColorOperation());

                pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = BlendStateMetal::getBlendFactor(blendState->getAlphaBlendSource());
                pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = BlendStateMetal::getBlendFactor(blendState->getAlphaBlendDest());
                pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = BlendStateMetal::getBlendOperation(blendState->getAlphaOperation());
            }

            pipelineStateDescriptor.colorAttachments[0].writeMask = MTLColorWriteMaskAll;

            NSError* error = Nil;
            id<MTLRenderPipelineState> pipelineState = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
            [pipelineStateDescriptor release];
            if (error || !pipelineState)
            {
                log("Failed to created Metal pipeline state");
                return Nil;
            }

            _pipelineStates[std::make_pair(blendState, shader)] = pipelineState;

            return pipelineState;
        }

    } // namespace video
} // namespace ouzel
