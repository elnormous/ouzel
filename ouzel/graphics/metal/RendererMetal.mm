// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#include "RendererMetal.h"
#include "TextureMetal.h"
#include "ShaderMetal.h"
#include "MeshBufferMetal.h"
#include "IndexBufferMetal.h"
#include "VertexBufferMetal.h"
#include "BlendStateMetal.h"
#include "events/EventDispatcher.h"
#if OUZEL_PLATFORM_MACOS
    #include "core/macos/WindowMacOS.h"
    #include "ColorPSMacOS.h"
    #include "ColorVSMacOS.h"
    #include "TexturePSMacOS.h"
    #include "TextureVSMacOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSMacOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSMacOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSMacOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSMacOS_metallib
#elif OUZEL_PLATFORM_TVOS
    #include "core/tvos/WindowTVOS.h"
    #include "ColorPSTVOS.h"
    #include "ColorVSTVOS.h"
    #include "TexturePSTVOS.h"
    #include "TextureVSTVOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSTVOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSTVOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSTVOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSTVOS_metallib
#elif OUZEL_PLATFORM_IOS
    #include "core/ios/WindowIOS.h"
    #include "ColorPSIOS.h"
    #include "ColorVSIOS.h"
    #include "TexturePSIOS.h"
    #include "TextureVSIOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSIOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSIOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSIOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSIOS_metallib
#endif
#include "core/Engine.h"
#include "core/Cache.h"
#include "utils/Log.h"
#include "stb_image_write.h"

namespace ouzel
{
    namespace graphics
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
            apiMajorVersion = 1;
            apiMinorVersion = 0;

            std::fill(std::begin(depthStencilStates), std::end(depthStencilStates), Nil);
        }

        RendererMetal::~RendererMetal()
        {
            for (uint32_t state = 0; state < 4; ++state)
            {
                if (depthStencilStates[state])
                {
                    [depthStencilStates[state] release];
                }
            }

            if (depthTexture)
            {
                [depthTexture release];
            }

            if (msaaTexture)
            {
                [msaaTexture release];
            }

            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder release];
            }

            if (currentCommandBuffer)
            {
                [currentCommandBuffer release];
            }

            for (const auto& pipelineState : pipelineStates)
            {
                [pipelineState.second release];
            }

            if (commandQueue)
            {
                [commandQueue release];
            }

            if (samplerState)
            {
                [samplerState release];
            }

            if (renderPassDescriptor)
            {
                [renderPassDescriptor release];
            }

            if (device)
            {
                [device release];
            }
        }

        void RendererMetal::free()
        {
            Renderer::free();

            for (uint32_t state = 0; state < 4; ++state)
            {
                if (depthStencilStates[state])
                {
                    [depthStencilStates[state] release];
                    depthStencilStates[state] = Nil;
                }
            }

            if (depthTexture)
            {
                [depthTexture release];
                depthTexture = Nil;
            }

            if (msaaTexture)
            {
                [msaaTexture release];
                msaaTexture = Nil;
            }

            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder release];
                currentRenderCommandEncoder = Nil;
            }

            if (currentCommandBuffer)
            {
                [currentCommandBuffer release];
                currentCommandBuffer = Nil;
            }

            for (const auto& pipelineState : pipelineStates)
            {
                [pipelineState.second release];
            }

            pipelineStates.clear();

            if (commandQueue)
            {
                [commandQueue release];
                commandQueue = Nil;
            }

            if (samplerState)
            {
                [samplerState release];
                samplerState = Nil;
            }

            if (renderPassDescriptor)
            {
                [renderPassDescriptor release];
                renderPassDescriptor = Nil;
            }

            if (device)
            {
                [device release];
                device = Nil;
            }
        }

        bool RendererMetal::init(Window* newWindow,
                                 const Size2& newSize,
                                 uint32_t newSampleCount,
                                 TextureResource::Filter newTextureFilter,
                                 PixelFormat newBackBufferFormat,
                                 bool newVerticalSync,
                                 bool newDepth)
        {
            if (!Renderer::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepth))
            {
                return false;
            }

            inflightSemaphore = dispatch_semaphore_create(3); // allow encoding up to 3 command buffers simultaneously

            device = MTLCreateSystemDefaultDevice();

            if (!device)
            {
                Log(Log::Level::ERR) << "Failed to create Metal device";
                return false;
            }

            if (device.name)
            {
                Log(Log::Level::INFO) << "Using " << [device.name cStringUsingEncoding:NSUTF8StringEncoding] << " for rendering";
            }

#if OUZEL_PLATFORM_MACOS
            view = (MTKViewPtr)static_cast<WindowMacOS*>(window)->getNativeView();
#elif OUZEL_PLATFORM_TVOS
            view = (MTKViewPtr)static_cast<WindowTVOS*>(window)->getNativeView();
#elif OUZEL_PLATFORM_IOS
            view = (MTKViewPtr)static_cast<WindowIOS*>(window)->getNativeView();
#endif
            view.device = device;
            view.sampleCount = sampleCount;
            view.framebufferOnly = NO; // for screenshot capturing

            colorFormat = view.colorPixelFormat;

            if (depth)
            {
                view.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
                depthFormat = view.depthStencilPixelFormat;

                for (uint32_t state = 0; state < 4; ++state)
                {
                    MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];
                    depthStencilDescriptor.depthCompareFunction = (state & 0x01) ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways; // read
                    depthStencilDescriptor.depthWriteEnabled = (state & 0x02) ? YES : NO; // write
                    depthStencilStates[state] = [device newDepthStencilStateWithDescriptor:depthStencilDescriptor];
                    [depthStencilDescriptor release];
                }
            }

            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
            {
                Log(Log::Level::ERR) << "Failed to create Metal render pass descriptor";
                return false;
            }

            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0f, 0.0f, 0.0f, 0.0f);
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
            renderPassDescriptor.depthAttachment.clearDepth = 1.0f;
            renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;

            commandQueue = [device newCommandQueue];

            if (!commandQueue)
            {
                Log(Log::Level::ERR) << "Failed to create Metal command queue";
                return false;
            }

            MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
            switch (textureFilter)
            {
                case TextureResource::Filter::NONE:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case TextureResource::Filter::LINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case TextureResource::Filter::BILINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case TextureResource::Filter::TRILINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
                    break;
            }
            samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
            samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;

            samplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];
            [samplerDescriptor release];

            if (!samplerState)
            {
                Log(Log::Level::ERR) << "Failed to create Metal sampler state";
                return false;
            }

            ShaderResourcePtr textureShader = createShader();
            textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_METAL), std::end(TEXTURE_PIXEL_SHADER_METAL)),
                                           std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_METAL), std::end(TEXTURE_VERTEX_SHADER_METAL)),
                                           VertexPCT::ATTRIBUTES,
                                           {{"color", ShaderResource::DataType::FLOAT_VECTOR4}},
                                           {{"modelViewProj", ShaderResource::DataType::FLOAT_MATRIX4}},
                                           256, 256,
                                           "main_ps", "main_vs");

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderResourcePtr colorShader = createShader();
            colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_METAL), std::end(COLOR_PIXEL_SHADER_METAL)),
                                         std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_METAL), std::end(COLOR_VERTEX_SHADER_METAL)),
                                         VertexPC::ATTRIBUTES,
                                         {{"color", ShaderResource::DataType::FLOAT_VECTOR4}},
                                         {{"modelViewProj", ShaderResource::DataType::FLOAT_MATRIX4}},
                                         256, 256,
                                         "main_ps", "main_vs");

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStateResourcePtr noBlendState = createBlendState();

            noBlendState->init(false,
                               BlendStateResource::BlendFactor::ONE, BlendStateResource::BlendFactor::ZERO,
                               BlendStateResource::BlendOperation::ADD,
                               BlendStateResource::BlendFactor::ONE, BlendStateResource::BlendFactor::ZERO,
                               BlendStateResource::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStateResourcePtr addBlendState = createBlendState();

            addBlendState->init(true,
                                BlendStateResource::BlendFactor::ONE, BlendStateResource::BlendFactor::ONE,
                                BlendStateResource::BlendOperation::ADD,
                                BlendStateResource::BlendFactor::ONE, BlendStateResource::BlendFactor::ONE,
                                BlendStateResource::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStateResourcePtr multiplyBlendState = createBlendState();

            multiplyBlendState->init(true,
                                     BlendStateResource::BlendFactor::DEST_COLOR, BlendStateResource::BlendFactor::ZERO,
                                     BlendStateResource::BlendOperation::ADD,
                                     BlendStateResource::BlendFactor::ONE, BlendStateResource::BlendFactor::ONE,
                                     BlendStateResource::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendStateResourcePtr alphaBlendState = createBlendState();

            alphaBlendState->init(true,
                                  BlendStateResource::BlendFactor::SRC_ALPHA, BlendStateResource::BlendFactor::INV_SRC_ALPHA,
                                  BlendStateResource::BlendOperation::ADD,
                                  BlendStateResource::BlendFactor::ONE, BlendStateResource::BlendFactor::ONE,
                                  BlendStateResource::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            TextureResourcePtr whitePixelTexture = createTexture();
            whitePixelTexture->initFromBuffer({255, 255, 255, 255}, Size2(1.0f, 1.0f), false, false);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            return true;
        }

        bool RendererMetal::update()
        {
            clearColorBuffer = uploadData.clearColorBuffer;
            clearDepthBuffer = uploadData.clearDepthBuffer;

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(uploadData.clearColor.normR(),
                                                                                    uploadData.clearColor.normG(),
                                                                                    uploadData.clearColor.normB(),
                                                                                    uploadData.clearColor.normA());

            return true;
        }

        bool RendererMetal::present()
        {
            if (!Renderer::present())
            {
                return false;
            }

            NSUInteger frameBufferWidth = view.currentDrawable.texture.width;
            NSUInteger frameBufferHeight = view.currentDrawable.texture.height;

            if (sampleCount > 1)
            {
                if (!msaaTexture ||
                    frameBufferWidth != msaaTexture.width ||
                    frameBufferHeight != msaaTexture.height)
                {
                    if (msaaTexture) [msaaTexture release];

                    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                                                                                    width: frameBufferWidth
                                                                                                   height: frameBufferHeight
                                                                                                mipmapped: NO];
                    desc.textureType = MTLTextureType2DMultisample;
                    desc.storageMode = MTLStorageModePrivate;
                    desc.sampleCount = sampleCount;
                    desc.usage = MTLTextureUsageRenderTarget;

                    msaaTexture = [device newTextureWithDescriptor: desc];

                    if (!msaaTexture)
                    {
                        Log(Log::Level::ERR) << "Failed to create MSAA texture";
                        return false;
                    }

                    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
                    renderPassDescriptor.colorAttachments[0].texture = msaaTexture;
                }

                renderPassDescriptor.colorAttachments[0].resolveTexture = view.currentDrawable.texture;
            }
            else
            {
                renderPassDescriptor.colorAttachments[0].texture = view.currentDrawable.texture;
            }

            if (depth)
            {
                if (!depthTexture ||
                    frameBufferWidth != depthTexture.width ||
                    frameBufferHeight != depthTexture.height)
                {
                    if (depthTexture) [depthTexture release];

                    MTLTextureDescriptor* desc = [MTLTextureDescriptor
                                                  texture2DDescriptorWithPixelFormat:static_cast<MTLPixelFormat>(depthFormat)
                                                  width:frameBufferWidth height:frameBufferHeight mipmapped:NO];

                    desc.textureType = (sampleCount > 1) ? MTLTextureType2DMultisample : MTLTextureType2D;
                    desc.storageMode = MTLStorageModePrivate;
                    desc.sampleCount = sampleCount;
                    desc.usage = MTLTextureUsageRenderTarget;

                    depthTexture = [device newTextureWithDescriptor:desc];

                    if (!depthTexture)
                    {
                        Log(Log::Level::ERR) << "Failed to create depth texture";
                        return false;
                    }

                    renderPassDescriptor.depthAttachment.texture = depthTexture;
                }
            }
            else
            {
                renderPassDescriptor.depthAttachment.texture = Nil;
            }

            dispatch_semaphore_wait(inflightSemaphore, DISPATCH_TIME_FOREVER);

            currentCommandBuffer = [[commandQueue commandBuffer] retain];

            if (!currentCommandBuffer)
            {
                Log(Log::Level::ERR) << "Failed to create Metal command buffer";
                return false;
            }

            __block dispatch_semaphore_t blockSemaphore = inflightSemaphore;
            [currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer>)
             {
                 dispatch_semaphore_signal(blockSemaphore);
             }];

            MTLScissorRect scissorRect;

            if (!update())
            {
                return false;
            }

            std::vector<float> shaderData;

            MTLViewport viewport;

            if (drawQueue.empty())
            {
                frameBufferClearedFrame = currentFrame;

                if (!createRenderCommandEncoder(renderPassDescriptor))
                {
                    return false;
                }

                viewport = {
                    0.0, 0.0,
                    static_cast<double>(frameBufferWidth),
                    static_cast<double>(frameBufferHeight),
                    0.0, 1.0
                };

                [currentRenderCommandEncoder setViewport: viewport];

                if (depthStencilStates[3])
                {
                    [currentRenderCommandEncoder setDepthStencilState:depthStencilStates[3]];
                }

                currentRenderPassDescriptor.colorAttachments[0].loadAction = clearColorBuffer ? MTLLoadActionClear : MTLLoadActionLoad;
                currentRenderPassDescriptor.depthAttachment.loadAction = clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionLoad;
            }
            else for (const DrawCommand& drawCommand : drawQueue)
            {
                MTLRenderPassDescriptorPtr newRenderPassDescriptor;
                uint32_t newSampleCount;
                NSUInteger newColorFormat;
                NSUInteger newDepthFormat;
                bool newClearColorBuffer = false;
                bool newClearDepthBuffer = false;

                viewport = {
                    static_cast<double>(drawCommand.viewport.position.v[0]),
                    static_cast<double>(drawCommand.viewport.position.v[1]),
                    static_cast<double>(drawCommand.viewport.size.v[0]),
                    static_cast<double>(drawCommand.viewport.size.v[1]),
                    0.0, 1.0
                };

                // render target
                if (drawCommand.renderTarget)
                {
                    std::shared_ptr<TextureMetal> renderTargetMetal = std::static_pointer_cast<TextureMetal>(drawCommand.renderTarget);

                    if (!renderTargetMetal->getRenderPassDescriptor())
                    {
                        continue;
                    }

                    newRenderPassDescriptor = renderTargetMetal->getRenderPassDescriptor();
                    newSampleCount = renderTargetMetal->getSampleCount();
                    newColorFormat = renderTargetMetal->getColorFormat();
                    newDepthFormat = renderTargetMetal->getDepthFormat();

                    std::shared_ptr<TextureMetal> renderTargetTextureMetal = std::static_pointer_cast<TextureMetal>(renderTargetMetal);
                    viewport.originY = renderTargetTextureMetal->getSize().v[1] - (viewport.originY + viewport.height);

                    scissorRect.x = scissorRect.y = 0;
                    scissorRect.width = renderTargetTextureMetal->getTexture().width;
                    scissorRect.height = renderTargetTextureMetal->getTexture().height;

                    if (renderTargetMetal->getFrameBufferClearedFrame() != currentFrame)
                    {
                        renderTargetMetal->setFrameBufferClearedFrame(currentFrame);
                        newClearColorBuffer = renderTargetMetal->getClearColorBuffer();
                        newClearDepthBuffer = renderTargetMetal->getClearDepthBuffer();
                    }
                }
                else
                {
                    newRenderPassDescriptor = renderPassDescriptor;
                    newSampleCount = sampleCount;
                    newColorFormat = colorFormat;
                    newDepthFormat = depthFormat;

                    viewport.originY = static_cast<float>(frameBufferHeight) - (viewport.originY + viewport.height);

                    scissorRect.x = scissorRect.y = 0;
                    scissorRect.width = frameBufferWidth;
                    scissorRect.height = frameBufferHeight;

                    if (frameBufferClearedFrame != currentFrame)
                    {
                        frameBufferClearedFrame = currentFrame;
                        newClearColorBuffer = clearColorBuffer;
                        newClearDepthBuffer = clearDepthBuffer;
                    }
                }

                if (currentRenderPassDescriptor != newRenderPassDescriptor ||
                    !currentRenderCommandEncoder)
                {
                    if (!createRenderCommandEncoder(newRenderPassDescriptor))
                    {
                        return false;
                    }

                    currentRenderPassDescriptor.colorAttachments[0].loadAction = newClearColorBuffer ? MTLLoadActionClear : MTLLoadActionLoad;
                    currentRenderPassDescriptor.depthAttachment.loadAction = newClearDepthBuffer ? MTLLoadActionClear : MTLLoadActionLoad;
                }

                [currentRenderCommandEncoder setViewport: viewport];

                [currentRenderCommandEncoder setTriangleFillMode:drawCommand.wireframe ? MTLTriangleFillModeLines : MTLTriangleFillModeFill];

                // scissor test
                if (drawCommand.scissorTestEnabled)
                {
                    scissorRect.x = static_cast<NSUInteger>(drawCommand.scissorTest.position.v[0]);
                    scissorRect.y = static_cast<NSUInteger>(drawCommand.scissorTest.position.v[1]);
                    scissorRect.width = static_cast<NSUInteger>(drawCommand.scissorTest.size.v[0]);
                    scissorRect.height = static_cast<NSUInteger>(drawCommand.scissorTest.size.v[1]);
                }

                [currentRenderCommandEncoder setScissorRect: scissorRect];

                uint32_t depthStencilStateIndex = 0;
                if (drawCommand.depthTest) depthStencilStateIndex |= 0x01;
                if (drawCommand.depthWrite) depthStencilStateIndex |= 0x02;

                if (depthStencilStates[depthStencilStateIndex])
                {
                    [currentRenderCommandEncoder setDepthStencilState:depthStencilStates[depthStencilStateIndex]];
                }

                // shader
                std::shared_ptr<ShaderMetal> shaderMetal = std::static_pointer_cast<ShaderMetal>(drawCommand.shader);

                if (!shaderMetal || !shaderMetal->getPixelShader() || !shaderMetal->getVertexShader())
                {
                    // don't render if invalid shader
                    continue;
                }

                // pixel shader constants
                const std::vector<ShaderMetal::Location>& pixelShaderConstantLocations = shaderMetal->getPixelShaderConstantLocations();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                    return false;
                }

                shaderData.clear();

                for (size_t i = 0; i < drawCommand.pixelShaderConstants.size(); ++i)
                {
                    const ShaderMetal::Location& pixelShaderConstantLocation = pixelShaderConstantLocations[i];
                    const std::vector<float>& pixelShaderConstant = drawCommand.pixelShaderConstants[i];

                    if (sizeof(float) * pixelShaderConstant.size() != pixelShaderConstantLocation.size)
                    {
                        Log(Log::Level::ERR) << "Invalid pixel shader constant size";
                        return false;
                    }

                    shaderData.insert(shaderData.end(), pixelShaderConstant.begin(), pixelShaderConstant.end());
                }

                shaderMetal->uploadBuffer(shaderMetal->getPixelShaderConstantBuffer(),
                                          shaderMetal->getPixelShaderConstantBufferOffset(),
                                          shaderData.data(),
                                          static_cast<uint32_t>(sizeof(float) * shaderData.size()));

                [currentRenderCommandEncoder setFragmentBuffer:shaderMetal->getPixelShaderConstantBuffer()
                                                        offset:shaderMetal->getPixelShaderConstantBufferOffset()
                                                       atIndex:1];

                // vertex shader constants
                const std::vector<ShaderMetal::Location>& vertexShaderConstantLocations = shaderMetal->getVertexShaderConstantLocations();

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                {
                    Log(Log::Level::ERR) << "Invalid vertex shader constant size";
                    return false;
                }

                shaderData.clear();

                for (size_t i = 0; i < drawCommand.vertexShaderConstants.size(); ++i)
                {
                    const ShaderMetal::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                    const std::vector<float>& vertexShaderConstant = drawCommand.vertexShaderConstants[i];

                    if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                    {
                        Log(Log::Level::ERR) << "Invalid vertex shader constant size";
                        return false;
                    }

                    shaderData.insert(shaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                }

                shaderMetal->uploadBuffer(shaderMetal->getVertexShaderConstantBuffer(),
                                          shaderMetal->getVertexShaderConstantBufferOffset(),
                                          shaderData.data(),
                                          static_cast<uint32_t>(sizeof(float) * shaderData.size()));

                [currentRenderCommandEncoder setVertexBuffer:shaderMetal->getVertexShaderConstantBuffer()
                                                      offset:shaderMetal->getVertexShaderConstantBufferOffset()
                                                     atIndex:1];

                shaderMetal->nextBuffers();

                // blend state
                std::shared_ptr<BlendStateMetal> blendStateMetal = std::static_pointer_cast<BlendStateMetal>(drawCommand.blendState);

                if (!blendStateMetal)
                {
                    // don't render if invalid blend state
                    continue;
                }

                PipelineStateDesc pipelineStateDesc = {
                    blendStateMetal,
                    shaderMetal,
                    newSampleCount,
                    newColorFormat,
                    newDepthFormat,
                };

                auto pipelineStateIterator = pipelineStates.find(pipelineStateDesc);

                if (pipelineStateIterator != pipelineStates.end())
                {
                    [currentRenderCommandEncoder setRenderPipelineState:pipelineStateIterator->second];
                }
                else
                {
                    id<MTLRenderPipelineState> pipelineState = createPipelineState(pipelineStateDesc);

                    if (!pipelineState)
                    {
                        return false;
                    }

                    [currentRenderCommandEncoder setRenderPipelineState:pipelineState];
                }

                bool texturesValid = true;

                // textures
                for (uint32_t layer = 0; layer < TextureResource::LAYERS; ++layer)
                {
                    std::shared_ptr<TextureMetal> textureMetal;

                    if (drawCommand.textures.size() > layer)
                    {
                        textureMetal = std::static_pointer_cast<TextureMetal>(drawCommand.textures[layer]);
                    }

                    if (textureMetal)
                    {
                        if (!textureMetal->getTexture())
                        {
                            texturesValid = false;
                            break;
                        }

                        [currentRenderCommandEncoder setFragmentTexture:textureMetal->getTexture() atIndex:layer];
                        [currentRenderCommandEncoder setFragmentSamplerState:samplerState atIndex:layer];
                    }
                    else
                    {
                        [currentRenderCommandEncoder setFragmentTexture:Nil atIndex:layer];
                    }
                }

                if (!texturesValid)
                {
                    continue;
                }

                // mesh buffer
                std::shared_ptr<MeshBufferMetal> meshBufferMetal = std::static_pointer_cast<MeshBufferMetal>(drawCommand.meshBuffer);

                if (!meshBufferMetal)
                {
                    // don't render if invalid mesh buffer
                    continue;
                }

                std::shared_ptr<IndexBufferMetal> indexBufferMetal = std::static_pointer_cast<IndexBufferMetal>(meshBufferMetal->getIndexBuffer());
                std::shared_ptr<VertexBufferMetal> vertexBufferMetal = std::static_pointer_cast<VertexBufferMetal>(meshBufferMetal->getVertexBuffer());

                if (!indexBufferMetal || !indexBufferMetal->getBuffer() ||
                    !vertexBufferMetal || !vertexBufferMetal->getBuffer())
                {
                    continue;
                }

                [currentRenderCommandEncoder setVertexBuffer:vertexBufferMetal->getBuffer() offset:0 atIndex:0];

                // draw
                MTLPrimitiveType primitiveType;

                switch (drawCommand.drawMode)
                {
                    case DrawMode::POINT_LIST: primitiveType = MTLPrimitiveTypePoint; break;
                    case DrawMode::LINE_LIST: primitiveType = MTLPrimitiveTypeLine; break;
                    case DrawMode::LINE_STRIP: primitiveType = MTLPrimitiveTypeLineStrip; break;
                    case DrawMode::TRIANGLE_LIST: primitiveType = MTLPrimitiveTypeTriangle; break;
                    case DrawMode::TRIANGLE_STRIP: primitiveType = MTLPrimitiveTypeTriangleStrip; break;
                    default: Log(Log::Level::ERR) << "Invalid draw mode"; return false;
                }

                [currentRenderCommandEncoder drawIndexedPrimitives:primitiveType
                                                        indexCount:drawCommand.indexCount
                                                         indexType:indexBufferMetal->getType()
                                                       indexBuffer:indexBufferMetal->getBuffer()
                                                 indexBufferOffset:drawCommand.startIndex * indexBufferMetal->getBytesPerIndex()];
            }

            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder endEncoding];
                [currentRenderCommandEncoder release];
                currentRenderCommandEncoder = Nil;
            }

            if (currentCommandBuffer)
            {
                [currentCommandBuffer presentDrawable:static_cast<id<CAMetalDrawable> _Nonnull>(view.currentDrawable)];

                [currentCommandBuffer commit];
                [currentCommandBuffer release];
                currentCommandBuffer = Nil;
            }

            return true;
        }

        std::vector<Size2> RendererMetal::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        BlendStateResourcePtr RendererMetal::createBlendState()
        {
            std::shared_ptr<BlendStateMetal> blendState = std::make_shared<BlendStateMetal>();
            return blendState;
        }

        TextureResourcePtr RendererMetal::createTexture()
        {
            std::shared_ptr<TextureMetal> texture(new TextureMetal());
            return texture;
        }

        ShaderResourcePtr RendererMetal::createShader()
        {
            std::shared_ptr<ShaderMetal> shader(new ShaderMetal());
            return shader;
        }

        MeshBufferResourcePtr RendererMetal::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferMetal> meshBuffer = std::make_shared<MeshBufferMetal>();
            return meshBuffer;
        }

        IndexBufferResourcePtr RendererMetal::createIndexBuffer()
        {
            std::shared_ptr<IndexBufferMetal> meshBuffer = std::make_shared<IndexBufferMetal>();
            return meshBuffer;
        }

        VertexBufferResourcePtr RendererMetal::createVertexBuffer()
        {
            std::shared_ptr<VertexBufferMetal> meshBuffer = std::make_shared<VertexBufferMetal>();
            return meshBuffer;
        }

        MTLRenderPipelineStatePtr RendererMetal::createPipelineState(const PipelineStateDesc& desc)
        {
            MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.sampleCount = desc.sampleCount;
            pipelineStateDescriptor.vertexFunction = desc.shader->getVertexShader();
            pipelineStateDescriptor.fragmentFunction = desc.shader->getPixelShader();
            pipelineStateDescriptor.vertexDescriptor = desc.shader->getVertexDescriptor();

            pipelineStateDescriptor.colorAttachments[0].pixelFormat = static_cast<MTLPixelFormat>(desc.colorFormat);
            pipelineStateDescriptor.depthAttachmentPixelFormat = static_cast<MTLPixelFormat>(desc.depthFormat);
            pipelineStateDescriptor.stencilAttachmentPixelFormat = MTLPixelFormatInvalid;

            // blending
            std::shared_ptr<BlendStateMetal> blendStateMetal = std::static_pointer_cast<BlendStateMetal>(desc.blendState);

            pipelineStateDescriptor.colorAttachments[0].blendingEnabled = desc.blendState->isMetalBlendingEnabled() ? YES : NO;

            pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = blendStateMetal->getSourceRGBBlendFactor();
            pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = blendStateMetal->getDestinationRGBBlendFactor();
            pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = blendStateMetal->getRGBBlendOperation();

            pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = blendStateMetal->getSourceAlphaBlendFactor();
            pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = blendStateMetal->getDestinationAlphaBlendFactor();
            pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = blendStateMetal->getAlphaBlendOperation();

            pipelineStateDescriptor.colorAttachments[0].writeMask = MTLColorWriteMaskAll;

            NSError* error = Nil;
            id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
            [pipelineStateDescriptor release];
            if (error || !pipelineState)
            {
                if (pipelineState) [pipelineState release];

                Log(Log::Level::ERR) << "Failed to created Metal pipeline state";
                return Nil;
            }

            pipelineStates[desc] = pipelineState;

            return pipelineState;
        }

        bool RendererMetal::generateScreenshot(const std::string& filename)
        {
            MTLTextureResourcePtr texture = view.currentDrawable.texture;

            if (!texture)
            {
                return false;
            }

            NSUInteger width = static_cast<NSUInteger>(texture.width);
            NSUInteger height = static_cast<NSUInteger>(texture.height);

            std::shared_ptr<uint8_t> data(new uint8_t[width * height * 4]);
            [texture getBytes:data.get() bytesPerRow:width * 4 fromRegion:MTLRegionMake2D(0, 0, width, height) mipmapLevel:0];

            uint8_t temp;
            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    temp = data.get()[((y * width + x) * 4)];
                    data.get()[((y * width + x) * 4)] = data.get()[((y * width + x) * 4) + 2];
                    data.get()[((y * width + x) * 4) + 2] = temp;
                    data.get()[((y * width + x) * 4) + 3] = 255;
                }
            }

            if (!stbi_write_png(filename.c_str(), static_cast<int>(width), static_cast<int>(height), 4, data.get(), static_cast<int>(width * 4)))
            {
                Log(Log::Level::ERR) << "Failed to save image to file";
                return false;
            }

            return true;
        }

        bool RendererMetal::createRenderCommandEncoder(MTLRenderPassDescriptorPtr newRenderPassDescriptor)
        {
            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder endEncoding];
                [currentRenderCommandEncoder release];
            }

            currentRenderPassDescriptor = newRenderPassDescriptor;
            currentRenderCommandEncoder = [[currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor] retain];

            if (!currentRenderCommandEncoder)
            {
                Log(Log::Level::ERR) << "Failed to create Metal render command encoder";
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
