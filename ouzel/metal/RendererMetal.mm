// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#include "RendererMetal.h"
#include "TextureMetal.h"
#include "RenderTargetMetal.h"
#include "ShaderMetal.h"
#include "MeshBufferMetal.h"
#include "BlendStateMetal.h"
#if OUZEL_PLATFORM_MACOS
    #include "macos/WindowMacOS.h"
    #include "ColorPSMacOS.h"
    #include "ColorVSMacOS.h"
    #include "TexturePSMacOS.h"
    #include "TextureVSMacOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSMacOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSMacOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSMacOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSMacOS_metallib
#elif OUZEL_PLATFORM_TVOS
    #include "tvos/WindowTVOS.h"
    #include "ColorPSTVOS.h"
    #include "ColorVSTVOS.h"
    #include "TexturePSTVOS.h"
    #include "TextureVSTVOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSTVOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSTVOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSTVOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSTVOS_metallib
#elif OUZEL_PLATFORM_IOS
    #include "ios/WindowIOS.h"
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
#include "utils/Utils.h"
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
            Renderer(Driver::METAL), dirty(false)
        {
            apiVersion = 1;
        }

        RendererMetal::~RendererMetal()
        {
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

            for (auto pipelineState : pipelineStates)
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

            for (auto pipelineState : pipelineStates)
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

        bool RendererMetal::init(const WindowPtr& window,
                                 uint32_t newSampleCount,
                                 TextureFiltering newTextureFiltering,
                                 float newTargetFPS,
                                 bool newVerticalSync)
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            
            if (!Renderer::init(window, newSampleCount, newTextureFiltering, newTargetFPS, newVerticalSync))
            {
                return false;
            }

            free();

            inflightSemaphore = dispatch_semaphore_create(3); // allow encoding up to 3 command buffers simultaneously

            device = MTLCreateSystemDefaultDevice();

            if (!device)
            {
                log("Failed to create Metal device");
                return false;
            }
#if OUZEL_PLATFORM_MACOS
            view = (MTKViewPtr)std::static_pointer_cast<WindowMacOS>(window)->getNativeView();
#elif OUZEL_PLATFORM_TVOS
            view = (MTKViewPtr)std::static_pointer_cast<WindowTVOS>(window)->getNativeView();
#elif OUZEL_PLATFORM_IOS
            view = (MTKViewPtr)std::static_pointer_cast<WindowIOS>(window)->getNativeView();
#endif
            view.device = device;
            view.sampleCount = sampleCount;
            view.framebufferOnly = NO; // for screenshot capturing
            //_view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
            {
                log("Failed to create Metal render pass descriptor");
                return false;
            }

            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

            commandQueue = [device newCommandQueue];

            if (!commandQueue)
            {
                log("Failed to create Metal command queue");
                return false;
            }

            MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
            samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
            switch (textureFiltering)
            {
                case TextureFiltering::NONE:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case TextureFiltering::LINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case TextureFiltering::BILINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
                    break;
                case TextureFiltering::TRILINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
                    break;
            }
            samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
            samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;

            samplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];
            [samplerDescriptor release];

            if (!samplerState)
            {
                log("Failed to create Metal sampler state");
                return false;
            }

            ShaderPtr textureShader = createShader();
            textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_METAL), std::end(TEXTURE_PIXEL_SHADER_METAL)),
                                           std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_METAL), std::end(TEXTURE_VERTEX_SHADER_METAL)),
                                           VertexPCT::ATTRIBUTES, "main_ps", "main_vs");

            textureShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}}, 256);
            textureShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = createShader();
            colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_METAL), std::end(COLOR_PIXEL_SHADER_METAL)),
                                         std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_METAL), std::end(COLOR_VERTEX_SHADER_METAL)),
                                         VertexPC::ATTRIBUTES, "main_ps", "main_vs");

            colorShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}}, 256);
            colorShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStatePtr noBlendState = createBlendState();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStatePtr addBlendState = createBlendState();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStatePtr multiplyBlendState = createBlendState();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendStatePtr alphaBlendState = createBlendState();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            ready = true;

            return true;
        }

        bool RendererMetal::update()
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());

                dirty = false;
            }
            
            return true;
        }

        void RendererMetal::setClearColor(Color newColor)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Renderer::setClearColor(newColor);

            dirty = true;
        }

        void RendererMetal::setSize(const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Renderer::setSize(newSize);

            dirty = true;
        }

        bool RendererMetal::present()
        {
            if (!Renderer::present())
            {
                return false;
            }

            clearedRenderPassDescriptors.clear();

            if (sampleCount > 1)
            {
                if (!msaaTexture ||
                    view.currentDrawable.texture.width != msaaTexture.width ||
                    view.currentDrawable.texture.height != msaaTexture.height)
                {
                    if (msaaTexture)
                    {
                        [msaaTexture release];
                        msaaTexture = Nil;
                    }

                    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                                                                                    width: view.currentDrawable.texture.width
                                                                                                   height: view.currentDrawable.texture.height
                                                                                                mipmapped: NO];
                    desc.textureType = MTLTextureType2DMultisample;
                    desc.storageMode = MTLStorageModePrivate;
                    desc.sampleCount = sampleCount;
                    desc.usage = MTLTextureUsageRenderTarget;

                    msaaTexture = [device newTextureWithDescriptor: desc];

                    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
                    renderPassDescriptor.colorAttachments[0].texture = msaaTexture;
                }

                renderPassDescriptor.colorAttachments[0].resolveTexture = view.currentDrawable.texture;
            }
            else
            {
                renderPassDescriptor.colorAttachments[0].texture = view.currentDrawable.texture;
            }

            dispatch_semaphore_wait(inflightSemaphore, DISPATCH_TIME_FOREVER);

            currentCommandBuffer = [[commandQueue commandBuffer] retain];

            if (!currentCommandBuffer)
            {
                log("Failed to create Metal command buffer");
                return false;
            }

            __block dispatch_semaphore_t blockSemaphore = inflightSemaphore;
            [currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer>)
             {
                 dispatch_semaphore_signal(blockSemaphore);
             }];

            bool previousScissorTestEnabled = false;
            Rectangle previousScissorTest;

            std::queue<DrawCommand> drawCommands;

            {
                std::lock_guard<std::mutex> lock(drawQueueMutex);
                drawCommands = drawQueue;
            }

            std::queue<ResourcePtr> resources;

            {
                std::lock_guard<std::mutex> lock(updateMutex);
                resources = std::move(updateQueue);
                updateSet.clear();
            }

            while (!resources.empty())
            {
                const ResourcePtr& resource = resources.front();

                if (!resource->update())
                {
                    return false;
                }

                resources.pop();
            }

            if (!update())
            {
                return false;
            }

            if (drawCommands.empty())
            {
                if (!createRenderCommandEncoder(renderPassDescriptor))
                {
                    return false;
                }
            }
            else while (!drawCommands.empty())
            {
                const DrawCommand& drawCommand = drawCommands.front();

                MTLRenderPassDescriptorPtr newRenderPassDescriptor = Nil;

                // render target
                if (drawCommand.renderTarget)
                {
                    std::shared_ptr<RenderTargetMetal> renderTargetMetal = std::static_pointer_cast<RenderTargetMetal>(drawCommand.renderTarget);

                    if (!renderTargetMetal->isReady())
                    {
                        // don't render if invalid render target
                        continue;
                    }

                    newRenderPassDescriptor = renderTargetMetal->getRenderPassDescriptor();
                }
                else
                {
                    newRenderPassDescriptor = renderPassDescriptor;
                }

                if (currentRenderPassDescriptor != newRenderPassDescriptor ||
                    !currentRenderCommandEncoder ||
                    drawCommand.scissorTestEnabled != previousScissorTestEnabled || // scissor test flag changed
                    (drawCommand.scissorTestEnabled && drawCommand.scissorTest != previousScissorTest)) // scissor test enabled and rectangles differ
                {
                    if (!createRenderCommandEncoder(newRenderPassDescriptor))
                    {
                        return false;
                    }
                }

                // scissor test
                if (drawCommand.scissorTestEnabled)
                {
                    MTLScissorRect rect;
                    rect.x = static_cast<NSUInteger>(drawCommand.scissorTest.x);
                    rect.y = static_cast<NSUInteger>(drawCommand.scissorTest.y);
                    rect.width = static_cast<NSUInteger>(drawCommand.scissorTest.width);
                    rect.height = static_cast<NSUInteger>(drawCommand.scissorTest.height);
                    [currentRenderCommandEncoder setScissorRect: rect];

                    previousScissorTestEnabled = drawCommand.scissorTestEnabled;
                    previousScissorTest = drawCommand.scissorTest;
                }

                // shader
                std::shared_ptr<ShaderMetal> shaderMetal = std::static_pointer_cast<ShaderMetal>(drawCommand.shader);

                if (!shaderMetal || !shaderMetal->isReady())
                {
                    // don't render if invalid shader
                    continue;
                }

                // pixel shader constants
                const std::vector<Shader::ConstantInfo>& pixelShaderConstantInfos = shaderMetal->getPixelShaderConstantInfo();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantInfos.size())
                {
                    log("Invalid pixel shader constant size");
                    return false;
                }

                std::vector<float> pixelShaderData;

                for (size_t i = 0; i < drawCommand.pixelShaderConstants.size(); ++i)
                {
                    const Shader::ConstantInfo& pixelShaderConstantInfo = pixelShaderConstantInfos[i];
                    const std::vector<float>& pixelShaderConstant = drawCommand.pixelShaderConstants[i];

                    if (vectorDataSize(pixelShaderConstant) != pixelShaderConstantInfo.size)
                    {
                        log("Invalid pixel shader constant size");
                        return false;
                    }

                    pixelShaderData.insert(pixelShaderData.end(), pixelShaderConstant.begin(), pixelShaderConstant.end());
                }

                shaderMetal->uploadData(shaderMetal->getPixelShaderConstantBuffer(),
                                        shaderMetal->getPixelShaderConstantBufferOffset(),
                                        pixelShaderData.data(),
                                        static_cast<uint32_t>(vectorDataSize(pixelShaderData)));

                [currentRenderCommandEncoder setFragmentBuffer:shaderMetal->getPixelShaderConstantBuffer()
                                                        offset:shaderMetal->getPixelShaderConstantBufferOffset()
                                                       atIndex:1];


                // vertex shader constants
                const std::vector<Shader::ConstantInfo>& vertexShaderConstantInfos = shaderMetal->getVertexShaderConstantInfo();

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantInfos.size())
                {
                    log("Invalid vertex shader constant size");
                    return false;
                }

                std::vector<float> vertexShaderData;

                for (size_t i = 0; i < drawCommand.vertexShaderConstants.size(); ++i)
                {
                    const Shader::ConstantInfo& vertexShaderConstantInfo = vertexShaderConstantInfos[i];
                    const std::vector<float>& vertexShaderConstant = drawCommand.vertexShaderConstants[i];

                    if (vectorDataSize(vertexShaderConstant) != vertexShaderConstantInfo.size)
                    {
                        log("Invalid vertex shader constant size");
                        return false;
                    }

                    vertexShaderData.insert(vertexShaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                }

                shaderMetal->uploadData(shaderMetal->getVertexShaderConstantBuffer(),
                                        shaderMetal->getVertexShaderConstantBufferOffset(),
                                        vertexShaderData.data(),
                                        static_cast<uint32_t>(vectorDataSize(vertexShaderData)));

                [currentRenderCommandEncoder setVertexBuffer:shaderMetal->getVertexShaderConstantBuffer()
                                                      offset:shaderMetal->getVertexShaderConstantBufferOffset()
                                                     atIndex:1];

                shaderMetal->nextBuffers();

                // blend state
                std::shared_ptr<BlendStateMetal> blendStateMetal = std::static_pointer_cast<BlendStateMetal>(drawCommand.blendState);

                if (!blendStateMetal || !blendStateMetal->isReady())
                {
                    // don't render if invalid blend state
                    continue;
                }

                auto pipelineStateIterator = pipelineStates.find(std::make_pair(blendStateMetal, shaderMetal));

                if (pipelineStateIterator != pipelineStates.end())
                {
                    [currentRenderCommandEncoder setRenderPipelineState:pipelineStateIterator->second];
                }
                else
                {
                    id<MTLRenderPipelineState> pipelineState = createPipelineState(blendStateMetal, shaderMetal);

                    if (!pipelineState)
                    {
                        return false;
                    }

                    [currentRenderCommandEncoder setRenderPipelineState:pipelineState];
                }

                // textures
                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    std::shared_ptr<TextureMetal> textureMetal;

                    if (drawCommand.textures.size() > layer)
                    {
                        textureMetal = std::static_pointer_cast<TextureMetal>(drawCommand.textures[layer]);
                    }

                    if (textureMetal)
                    {
                        if (!textureMetal->isReady())
                        {
                            // don't render if invalid texture
                            continue;
                        }

                        [currentRenderCommandEncoder setFragmentTexture:textureMetal->getTexture() atIndex:layer];
                    }
                    else
                    {
                        [currentRenderCommandEncoder setFragmentTexture:Nil atIndex:layer];
                    }
                }

                [currentRenderCommandEncoder setFragmentSamplerState:samplerState atIndex:0];

                // mesh buffer
                std::shared_ptr<MeshBufferMetal> meshBufferMetal = std::static_pointer_cast<MeshBufferMetal>(drawCommand.meshBuffer);

                if (!meshBufferMetal || !meshBufferMetal->isReady())
                {
                    // don't render if invalid mesh buffer
                    continue;
                }

                [currentRenderCommandEncoder setVertexBuffer:meshBufferMetal->getVertexBuffer() offset:0 atIndex:0];

                // draw
                MTLPrimitiveType primitiveType;

                switch (drawCommand.drawMode)
                {
                    case DrawMode::POINT_LIST: primitiveType = MTLPrimitiveTypePoint; break;
                    case DrawMode::LINE_LIST: primitiveType = MTLPrimitiveTypeLine; break;
                    case DrawMode::LINE_STRIP: primitiveType = MTLPrimitiveTypeLineStrip; break;
                    case DrawMode::TRIANGLE_LIST: primitiveType = MTLPrimitiveTypeTriangle; break;
                    case DrawMode::TRIANGLE_STRIP: primitiveType = MTLPrimitiveTypeTriangleStrip; break;
                    default: log("Invalid draw mode"); return false;
                }

                [currentRenderCommandEncoder drawIndexedPrimitives:primitiveType
                                                        indexCount:drawCommand.indexCount
                                                         indexType:meshBufferMetal->getIndexFormat()
                                                       indexBuffer:meshBufferMetal->getIndexBuffer()
                                                 indexBufferOffset:static_cast<NSUInteger>(drawCommand.startIndex * meshBufferMetal->getIndexSize())];

                drawCommands.pop();
            }

            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder endEncoding];
                [currentRenderCommandEncoder release];
                currentRenderCommandEncoder = Nil;
            }

            if (currentCommandBuffer)
            {
                [currentCommandBuffer presentDrawable:view.currentDrawable];

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

        BlendStatePtr RendererMetal::createBlendState()
        {
            std::shared_ptr<BlendStateMetal> blendState(new BlendStateMetal());
            return blendState;
        }

        TexturePtr RendererMetal::createTexture()
        {
            std::shared_ptr<TextureMetal> texture(new TextureMetal());
            return texture;
        }

        RenderTargetPtr RendererMetal::createRenderTarget()
        {
            std::shared_ptr<RenderTargetMetal> renderTarget(new RenderTargetMetal());
            return renderTarget;
        }

        ShaderPtr RendererMetal::createShader()
        {
            std::shared_ptr<ShaderMetal> shader(new ShaderMetal());
            return shader;
        }

        MeshBufferPtr RendererMetal::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferMetal> meshBuffer(new MeshBufferMetal());
            return meshBuffer;
        }

        MTLRenderPipelineStatePtr RendererMetal::createPipelineState(const std::shared_ptr<BlendStateMetal>& blendState,
                                                                     const std::shared_ptr<ShaderMetal>& shader)
        {
            MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.sampleCount = view.sampleCount;
            pipelineStateDescriptor.vertexFunction = shader->getVertexShader();
            pipelineStateDescriptor.fragmentFunction = shader->getPixelShader();
            pipelineStateDescriptor.vertexDescriptor = shader->getVertexDescriptor();
            pipelineStateDescriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
            pipelineStateDescriptor.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;

            pipelineStateDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;

            // blending
            std::shared_ptr<BlendStateMetal> blendStateMetal = std::static_pointer_cast<BlendStateMetal>(blendState);

            pipelineStateDescriptor.colorAttachments[0].blendingEnabled = blendState->isBlendingEnabled() ? YES : NO;

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

                log("Failed to created Metal pipeline state");
                return Nil;
            }

            pipelineStates[std::make_pair(blendState, shader)] = pipelineState;

            return pipelineState;
        }

        bool RendererMetal::saveScreenshot(const std::string& filename)
        {
            MTLTexturePtr texture = view.currentDrawable.texture;

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
                log("Failed to save image to file");
                return false;
            }

            return false;
        }

        bool RendererMetal::createRenderCommandEncoder(MTLRenderPassDescriptorPtr newRenderPassDescriptor)
        {
            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder endEncoding];
                [currentRenderCommandEncoder release];
            }

            currentRenderPassDescriptor = newRenderPassDescriptor;

            auto clearedRenderPassDescriptor = clearedRenderPassDescriptors.insert(currentRenderPassDescriptor);

            if (clearedRenderPassDescriptor.second)
            {
                currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            }
            else
            {
                currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
            }

            currentRenderCommandEncoder = [[currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor] retain];

            if (!currentRenderCommandEncoder)
            {
                log("Failed to create Metal render command encoder");
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
