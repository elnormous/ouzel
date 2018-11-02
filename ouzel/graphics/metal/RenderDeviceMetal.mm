// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#include <cassert>

#if OUZEL_COMPILE_METAL

#include "RenderDeviceMetal.hpp"
#include "BufferResourceMetal.hpp"
#include "RenderTargetResourceMetal.hpp"
#include "ShaderResourceMetal.hpp"
#include "TextureResourceMetal.hpp"
#include "BlendStateResourceMetal.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"
#include "stb_image_write.h"

namespace ouzel
{
    namespace graphics
    {
        bool RenderDeviceMetal::available()
        {
            id<MTLDevice> device = MTLCreateSystemDefaultDevice();

            if (device)
            {
                [device release];
                return true;
            }

            return false;
        }

        RenderDeviceMetal::RenderDeviceMetal():
            RenderDevice(Renderer::Driver::METAL)
        {
            apiMajorVersion = 1;
            apiMinorVersion = 0;

            std::fill(std::begin(depthStencilStates), std::end(depthStencilStates), nil);
        }

        RenderDeviceMetal::~RenderDeviceMetal()
        {
            resources.clear();

            for (const ShaderConstantBuffer& shaderConstantBuffer : shaderConstantBuffers)
                for (MTLBufferPtr buffer : shaderConstantBuffer.buffers)
                    [buffer release];

            for (id<MTLDepthStencilState> depthStencilState : depthStencilStates)
                if (depthStencilState) [depthStencilState release];

            for (const auto& samplerState : samplerStates)
                [samplerState.second release];

            if (depthTexture) [depthTexture release];

            if (msaaTexture) [msaaTexture release];

            for (const auto& pipelineState : pipelineStates)
                [pipelineState.second release];

            if (metalCommandQueue) [metalCommandQueue release];

            if (renderPassDescriptor) [renderPassDescriptor release];

            if (device) [device release];

            if (currentMetalTexture) [currentMetalTexture release];
        }

        void RenderDeviceMetal::init(Window* newWindow,
                                     const Size2& newSize,
                                     uint32_t newSampleCount,
                                     Texture::Filter newTextureFilter,
                                     uint32_t newMaxAnisotropy,
                                     bool newVerticalSync,
                                     bool newDepth,
                                     bool newDebugRenderer)
        {
            RenderDevice::init(newWindow,
                               newSize,
                               newSampleCount,
                               newTextureFilter,
                               newMaxAnisotropy,
                               newVerticalSync,
                               newDepth,
                               newDebugRenderer);

            inflightSemaphore = dispatch_semaphore_create(BUFFER_COUNT);

            device = MTLCreateSystemDefaultDevice();

            if (!device)
                throw SystemError("Failed to create Metal device");

            if (device.name)
                engine->log(Log::Level::INFO) << "Using " << [device.name cStringUsingEncoding:NSUTF8StringEncoding] << " for rendering";

            metalCommandQueue = [device newCommandQueue];

            if (!metalCommandQueue)
                throw SystemError("Failed to create Metal command queue");

            if (depth) depthFormat = MTLPixelFormatDepth32Float;

            uint32_t depthStencilStateIndex = 0;

            for (uint32_t depthEnable = 0; depthEnable < 2; ++depthEnable)
            {
                for (uint32_t depthWriteMask = 0; depthWriteMask < 2; ++depthWriteMask)
                {
                    MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];

                    depthStencilDescriptor.depthCompareFunction = (depthEnable == 0) ? MTLCompareFunctionAlways : MTLCompareFunctionLessEqual; // depth read
                    depthStencilDescriptor.depthWriteEnabled = (depthWriteMask == 0) ? NO : YES; // depth write
                    depthStencilStates[depthStencilStateIndex] = [device newDepthStencilStateWithDescriptor:depthStencilDescriptor];
                    [depthStencilDescriptor release];

                    ++depthStencilStateIndex;
                }
            }

            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
                throw SystemError("Failed to create Metal render pass descriptor");

            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.normR(),
                                                                                    clearColor.normG(),
                                                                                    clearColor.normB(),
                                                                                    clearColor.normA());
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
            renderPassDescriptor.depthAttachment.clearDepth = clearDepth;
            renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
        }

        void RenderDeviceMetal::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            colorBufferLoadAction = clearColorBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void RenderDeviceMetal::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            depthBufferLoadAction = clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void RenderDeviceMetal::setClearColor(Color newClearColor)
        {
            clearColor = newClearColor;

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.normR(),
                                                                                    clearColor.normG(),
                                                                                    clearColor.normB(),
                                                                                    clearColor.normA());
        }

        void RenderDeviceMetal::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;

            renderPassDescriptor.depthAttachment.clearDepth = clearDepth;
        }

        void RenderDeviceMetal::setSize(const Size2& newSize)
        {
            RenderDevice::setSize(newSize);

            CGSize drawableSize = CGSizeMake(size.width, size.height);
            metalLayer.drawableSize = drawableSize;
        }

        void RenderDeviceMetal::process()
        {
            RenderDevice::process();
            executeAll();

            id<CAMetalDrawable> currentMetalDrawable = [metalLayer nextDrawable];

            if (!currentMetalDrawable)
                throw DataError("Failed to get Metal drawable");

            if (currentMetalTexture) [currentMetalTexture release];

            currentMetalTexture = [currentMetalDrawable.texture retain];

            NSUInteger frameBufferWidth = currentMetalTexture.width;
            NSUInteger frameBufferHeight = currentMetalTexture.height;

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
                        throw DataError("Failed to create MSAA texture");

                    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
                    renderPassDescriptor.colorAttachments[0].texture = msaaTexture;
                }

                renderPassDescriptor.colorAttachments[0].resolveTexture = currentMetalTexture;
            }
            else
                renderPassDescriptor.colorAttachments[0].texture = currentMetalTexture;

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
                        throw DataError("Failed to create depth texture");

                    renderPassDescriptor.depthAttachment.texture = depthTexture;
                }
            }
            else
                renderPassDescriptor.depthAttachment.texture = nil;

            dispatch_semaphore_wait(inflightSemaphore, DISPATCH_TIME_FOREVER);

            id<MTLCommandBuffer> currentCommandBuffer = [metalCommandQueue commandBuffer];

            if (!currentCommandBuffer)
                throw DataError("Failed to create Metal command buffer");

            __block dispatch_semaphore_t blockSemaphore = inflightSemaphore;
            [currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer>)
             {
                 dispatch_semaphore_signal(blockSemaphore);
             }];

            MTLRenderPassDescriptorPtr currentRenderPassDescriptor = nil;
            id<MTLRenderCommandEncoder> currentRenderCommandEncoder = nil;
            PipelineStateDesc currentPipelineStateDesc;
            MTLTexturePtr currentRenderTarget = nil;
            std::vector<float> shaderData;

            if (++shaderConstantBufferIndex >= BUFFER_COUNT) shaderConstantBufferIndex = 0;
            ShaderConstantBuffer& shaderConstantBuffer = shaderConstantBuffers[shaderConstantBufferIndex];
            shaderConstantBuffer.index = 0;
            shaderConstantBuffer.offset = 0;
            ShaderResourceMetal* currentShader = nullptr;

            std::unique_ptr<Command> command;

            for (;;)
            {
                {
                    std::unique_lock<std::mutex> lock(commandQueueMutex);
                    while (commandQueue.empty()) commandQueueCondition.wait(lock);
                    command = std::move(commandQueue.front());
                    commandQueue.pop();
                }

                switch (command->type)
                {
                    case Command::Type::PRESENT:
                    {
                        if (currentRenderCommandEncoder)
                            [currentRenderCommandEncoder endEncoding];

                        if (currentCommandBuffer)
                        {
                            [currentCommandBuffer presentDrawable:currentMetalDrawable];
                            [currentCommandBuffer commit];
                        }
                        break;
                    }

                    case Command::Type::SET_RENDER_TARGET:
                    {
                        const SetRenderTargetCommand* setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                        MTLRenderPassDescriptorPtr newRenderPassDescriptor;

                        if (setRenderTargetCommand->renderTarget)
                        {
                            TextureResourceMetal* renderTargetMetal = static_cast<TextureResourceMetal*>(resources[setRenderTargetCommand->renderTarget - 1].get());

                            currentRenderTarget = renderTargetMetal->getTexture();
                            newRenderPassDescriptor = renderTargetMetal->getRenderPassDescriptor();
                            if (!newRenderPassDescriptor) break;

                            currentPipelineStateDesc.sampleCount = renderTargetMetal->getSampleCount();
                            currentPipelineStateDesc.colorFormat = renderTargetMetal->getColorFormat();
                            currentPipelineStateDesc.depthFormat = renderTargetMetal->getDepthFormat();

                        }
                        else
                        {
                            currentRenderTarget = currentMetalTexture;
                            newRenderPassDescriptor = renderPassDescriptor;
                            currentPipelineStateDesc.sampleCount = sampleCount;
                            currentPipelineStateDesc.colorFormat = colorFormat;
                            currentPipelineStateDesc.depthFormat = depthFormat;
                        }

                        if (currentRenderPassDescriptor != newRenderPassDescriptor ||
                            !currentRenderCommandEncoder)
                        {
                            if (currentRenderCommandEncoder)
                                [currentRenderCommandEncoder endEncoding];

                            currentRenderPassDescriptor = newRenderPassDescriptor;
                            currentRenderCommandEncoder = [currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor];

                            if (!currentRenderCommandEncoder)
                                throw DataError("Failed to create Metal render command encoder");

                            currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
                            currentRenderPassDescriptor.depthAttachment.loadAction = MTLLoadActionLoad;
                        }

                        break;
                    }

                    case Command::Type::SET_RENDER_TARGET_PARAMETERS:
                    {
                        const SetRenderTargetParametersCommand* setRenderTargetParametersCommand = static_cast<const SetRenderTargetParametersCommand*>(command.get());

                        if (setRenderTargetParametersCommand->renderTarget)
                        {
                            TextureResourceMetal* renderTargetMetal = static_cast<TextureResourceMetal*>(resources[setRenderTargetParametersCommand->renderTarget - 1].get());
                            renderTargetMetal->setClearColorBuffer(setRenderTargetParametersCommand->clearColorBuffer);
                            renderTargetMetal->setClearDepthBuffer(setRenderTargetParametersCommand->clearDepthBuffer);
                            renderTargetMetal->setClearColor(setRenderTargetParametersCommand->clearColor);
                            renderTargetMetal->setClearDepth(setRenderTargetParametersCommand->clearDepth);
                        }
                        else
                        {
                            setClearColorBuffer(setRenderTargetParametersCommand->clearColorBuffer);
                            setClearDepthBuffer(setRenderTargetParametersCommand->clearDepthBuffer);
                            setClearColor(setRenderTargetParametersCommand->clearColor);
                            setClearDepth(setRenderTargetParametersCommand->clearDepth);
                        }

                        break;
                    }

                    case Command::Type::CLEAR_RENDER_TARGET:
                    {
                        const ClearRenderTargetCommand* clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

                        MTLRenderPassDescriptorPtr newRenderPassDescriptor;
                        MTLLoadAction newColorBufferLoadAction = MTLLoadActionLoad;
                        MTLLoadAction newDepthBufferLoadAction = MTLLoadActionLoad;

                        NSUInteger renderTargetWidth = 0;
                        NSUInteger renderTargetHeight = 0;

                        // render target
                        if (clearCommand->renderTarget)
                        {
                            TextureResourceMetal* renderTargetMetal = static_cast<TextureResourceMetal*>(resources[clearCommand->renderTarget - 1].get());

                            newRenderPassDescriptor = renderTargetMetal->getRenderPassDescriptor();
                            if (!newRenderPassDescriptor) break;

                            currentRenderTarget = renderTargetMetal->getTexture();
                            currentPipelineStateDesc.sampleCount = renderTargetMetal->getSampleCount();
                            currentPipelineStateDesc.colorFormat = renderTargetMetal->getColorFormat();
                            currentPipelineStateDesc.depthFormat = renderTargetMetal->getDepthFormat();

                            renderTargetWidth = renderTargetMetal->getWidth();
                            renderTargetHeight = renderTargetMetal->getHeight();

                            newColorBufferLoadAction = renderTargetMetal->getColorBufferLoadAction();
                            newDepthBufferLoadAction = renderTargetMetal->getDepthBufferLoadAction();
                        }
                        else
                        {
                            currentRenderTarget = currentMetalTexture;
                            newRenderPassDescriptor = renderPassDescriptor;
                            currentPipelineStateDesc.sampleCount = sampleCount;
                            currentPipelineStateDesc.colorFormat = colorFormat;
                            currentPipelineStateDesc.depthFormat = depthFormat;

                            renderTargetWidth = frameBufferWidth;
                            renderTargetHeight = frameBufferHeight;

                            newColorBufferLoadAction = colorBufferLoadAction;
                            newDepthBufferLoadAction = depthBufferLoadAction;
                        }

                        if (currentRenderCommandEncoder)
                            [currentRenderCommandEncoder endEncoding];

                        currentRenderPassDescriptor = newRenderPassDescriptor;
                        currentRenderCommandEncoder = [currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor];

                        if (!currentRenderCommandEncoder)
                            throw DataError("Failed to create Metal render command encoder");

                        MTLViewport viewport;
                        viewport.originX = viewport.originY = 0.0;
                        viewport.width = static_cast<double>(renderTargetWidth);
                        viewport.height = static_cast<double>(renderTargetHeight);
                        viewport.znear = 0.0f;
                        viewport.zfar = 1.0f;

                        [currentRenderCommandEncoder setViewport: viewport];
                        [currentRenderCommandEncoder setDepthStencilState:depthStencilStates[1]]; // enable depth write

                        currentRenderPassDescriptor.colorAttachments[0].loadAction = newColorBufferLoadAction;
                        currentRenderPassDescriptor.depthAttachment.loadAction = newDepthBufferLoadAction;

                        break;
                    }

                    case Command::Type::BLIT:
                    {
                        //const BlitCommand* blitCommand = static_cast<const BlitCommand*>(command.get());
                        //MTLBlitCommandEncoder
                        break;
                    }

                    case Command::Type::SET_CULL_MODE:
                    {
                        const SetCullModeCommad* setCullModeCommad = static_cast<const SetCullModeCommad*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        MTLCullMode cullMode;

                        switch (setCullModeCommad->cullMode)
                        {
                            case Renderer::CullMode::NONE: cullMode = MTLCullModeNone; break;
                            case Renderer::CullMode::FRONT: cullMode = MTLCullModeFront; break;
                            case Renderer::CullMode::BACK: cullMode = MTLCullModeBack; break;
                            default: throw DataError("Invalid cull mode");
                        }

                        [currentRenderCommandEncoder setCullMode:cullMode];

                        break;
                    }

                    case Command::Type::SET_FILL_MODE:
                    {
                        const SetFillModeCommad* setFillModeCommad = static_cast<const SetFillModeCommad*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        MTLTriangleFillMode fillMode;

                        switch (setFillModeCommad->fillMode)
                        {
                            case Renderer::FillMode::SOLID: fillMode = MTLTriangleFillModeFill; break;
                            case Renderer::FillMode::WIREFRAME: fillMode = MTLTriangleFillModeLines; break;
                            default: throw DataError("Invalid fill mode");
                        }

                        [currentRenderCommandEncoder setTriangleFillMode:fillMode];

                        break;
                    }

                    case Command::Type::SET_SCISSOR_TEST:
                    {
                        const SetScissorTestCommand* setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        MTLScissorRect scissorRect;

                        if (setScissorTestCommand->enabled)
                        {
                            scissorRect.x = static_cast<NSUInteger>(setScissorTestCommand->rectangle.position.x);
                            scissorRect.y = static_cast<NSUInteger>(setScissorTestCommand->rectangle.position.y);
                            scissorRect.width = static_cast<NSUInteger>(setScissorTestCommand->rectangle.size.width);
                            scissorRect.height = static_cast<NSUInteger>(setScissorTestCommand->rectangle.size.height);
                            if (scissorRect.x >= currentRenderTarget.width) scissorRect.x = currentRenderTarget.width - 1;
                            if (scissorRect.y >= currentRenderTarget.height) scissorRect.y = currentRenderTarget.height - 1;
                            if (scissorRect.width > currentRenderTarget.width - scissorRect.x) scissorRect.width = currentRenderTarget.width - scissorRect.x;
                            if (scissorRect.height > currentRenderTarget.height - scissorRect.y) scissorRect.height = currentRenderTarget.height - scissorRect.y;
                        }
                        else
                        {
                            scissorRect.x = scissorRect.y = 0;
                            scissorRect.width = currentRenderTarget.width;
                            scissorRect.height = currentRenderTarget.height;
                        }

                        [currentRenderCommandEncoder setScissorRect: scissorRect];

                        break;
                    }

                    case Command::Type::SET_VIEWPORT:
                    {
                        const SetViewportCommand* setViewportCommand = static_cast<const SetViewportCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        MTLViewport viewport;
                        viewport.originX = static_cast<double>(setViewportCommand->viewport.position.x);
                        viewport.originY = static_cast<double>(setViewportCommand->viewport.position.y);
                        viewport.width = static_cast<double>(setViewportCommand->viewport.size.width);
                        viewport.height = static_cast<double>(setViewportCommand->viewport.size.height);
                        viewport.znear = 0.0f;
                        viewport.zfar = 1.0f;

                        [currentRenderCommandEncoder setViewport: viewport];

                        break;
                    }

                    case Command::Type::SET_DEPTH_STATE:
                    {
                        const SetDepthStateCommand* setDepthStateCommand = static_cast<const SetDepthStateCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        uint32_t depthTestIndex = setDepthStateCommand->depthTest ? 1 : 0;
                        uint32_t depthWriteIndex = setDepthStateCommand->depthWrite ? 1 : 0;
                        uint32_t depthStencilStateIndex = depthTestIndex * 2 + depthWriteIndex;

                        [currentRenderCommandEncoder setDepthStencilState:depthStencilStates[depthStencilStateIndex]];

                        break;
                    }

                    case Command::Type::SET_PIPELINE_STATE:
                    {
                        const SetPipelineStateCommand* setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        BlendStateResourceMetal* blendStateMetal = static_cast<BlendStateResourceMetal*>(resources[setPipelineStateCommand->blendState - 1].get());
                        ShaderResourceMetal* shaderMetal = static_cast<ShaderResourceMetal*>(resources[setPipelineStateCommand->shader - 1].get());
                        currentShader = shaderMetal;

                        currentPipelineStateDesc.blendState = blendStateMetal;
                        currentPipelineStateDesc.shader = shaderMetal;

                        MTLRenderPipelineStatePtr pipelineState = getPipelineState(currentPipelineStateDesc);
                        if (pipelineState) [currentRenderCommandEncoder setRenderPipelineState:pipelineState];

                        break;
                    }

                    case Command::Type::DRAW:
                    {
                        const DrawCommand* drawCommand = static_cast<const DrawCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        // mesh buffer
                        BufferResourceMetal* indexBufferMetal = static_cast<BufferResourceMetal*>(resources[drawCommand->indexBuffer - 1].get());
                        BufferResourceMetal* vertexBufferMetal = static_cast<BufferResourceMetal*>(resources[drawCommand->vertexBuffer - 1].get());

                        assert(indexBufferMetal);
                        assert(indexBufferMetal->getBuffer());
                        assert(vertexBufferMetal);
                        assert(vertexBufferMetal->getBuffer());

                        [currentRenderCommandEncoder setVertexBuffer:vertexBufferMetal->getBuffer() offset:0 atIndex:0];

                        // draw
                        MTLPrimitiveType primitiveType;

                        switch (drawCommand->drawMode)
                        {
                            case Renderer::DrawMode::POINT_LIST: primitiveType = MTLPrimitiveTypePoint; break;
                            case Renderer::DrawMode::LINE_LIST: primitiveType = MTLPrimitiveTypeLine; break;
                            case Renderer::DrawMode::LINE_STRIP: primitiveType = MTLPrimitiveTypeLineStrip; break;
                            case Renderer::DrawMode::TRIANGLE_LIST: primitiveType = MTLPrimitiveTypeTriangle; break;
                            case Renderer::DrawMode::TRIANGLE_STRIP: primitiveType = MTLPrimitiveTypeTriangleStrip; break;
                            default: throw DataError("Invalid draw mode");
                        }

                        assert(drawCommand->indexCount);
                        assert(indexBufferMetal->getSize());
                        assert(vertexBufferMetal->getSize());

                        MTLIndexType indexType;

                        switch (drawCommand->indexSize)
                        {
                            case 2: indexType = MTLIndexTypeUInt16; break;
                            case 4: indexType = MTLIndexTypeUInt32; break;
                            default: throw DataError("Invalid index size");
                        }

                        [currentRenderCommandEncoder drawIndexedPrimitives:primitiveType
                                                                indexCount:drawCommand->indexCount
                                                                 indexType:indexType
                                                               indexBuffer:indexBufferMetal->getBuffer()
                                                         indexBufferOffset:drawCommand->startIndex * drawCommand->indexSize];

                        break;
                    }

                    case Command::Type::PUSH_DEBUG_MARKER:
                    {
                        const PushDebugMarkerCommand* pushDebugMarkerCommand = static_cast<const PushDebugMarkerCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        [currentRenderCommandEncoder pushDebugGroup:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:pushDebugMarkerCommand->name.c_str()])];
                        break;
                    }

                    case Command::Type::POP_DEBUG_MARKER:
                    {
                        //const PopDebugMarkerCommand* popDebugMarkerCommand = static_cast<const PopDebugMarkerCommand*>(command);

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        [currentRenderCommandEncoder popDebugGroup];
                        break;
                    }

                    case Command::Type::INIT_BLEND_STATE:
                    {
                        const InitBlendStateCommand* initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

                        std::unique_ptr<BlendStateResourceMetal> blendStateResourceMetal(new BlendStateResourceMetal(*this));
                        blendStateResourceMetal->init(initBlendStateCommand->enableBlending,
                                                      initBlendStateCommand->colorBlendSource,
                                                      initBlendStateCommand->colorBlendDest,
                                                      initBlendStateCommand->colorOperation,
                                                      initBlendStateCommand->alphaBlendSource,
                                                      initBlendStateCommand->alphaBlendDest,
                                                      initBlendStateCommand->alphaOperation,
                                                      initBlendStateCommand->colorMask);

                        if (initBlendStateCommand->blendState > resources.size())
                            resources.resize(initBlendStateCommand->blendState);
                        resources[initBlendStateCommand->blendState - 1] = std::move(blendStateResourceMetal);
                        break;
                    }

                    case Command::Type::DELETE_BLEND_STATE:
                    {
                        const DeleteBlendStateCommand* deleteBlendStateCommand = static_cast<const DeleteBlendStateCommand*>(command.get());
                        resources[deleteBlendStateCommand->blendState - 1].reset();
                        break;
                    }

                    case Command::Type::INIT_BUFFER:
                    {
                        const InitBufferCommand* initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

                        std::unique_ptr<BufferResourceMetal> bufferResourceMetal(new BufferResourceMetal(*this));
                        bufferResourceMetal->init(initBufferCommand->usage,
                                                  initBufferCommand->flags,
                                                  initBufferCommand->data,
                                                  initBufferCommand->size);

                        if (initBufferCommand->buffer > resources.size())
                            resources.resize(initBufferCommand->buffer);
                        resources[initBufferCommand->buffer - 1] = std::move(bufferResourceMetal);
                        break;
                    }

                    case Command::Type::DELETE_BUFFER:
                    {
                        const DeleteBufferCommand* deleteBufferCommand = static_cast<const DeleteBufferCommand*>(command.get());
                        resources[deleteBufferCommand->buffer - 1].reset();
                        break;
                    }

                    case Command::Type::SET_BUFFER_DATA:
                    {
                        const SetBufferDataCommand* setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                        BufferResourceMetal* bufferResourceMetal = static_cast<BufferResourceMetal*>(resources[setBufferDataCommand->buffer - 1].get());
                        bufferResourceMetal->setData(setBufferDataCommand->data);
                        break;
                    }

                    case Command::Type::INIT_SHADER:
                    {
                        const InitShaderCommand* initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                        std::unique_ptr<ShaderResourceMetal> shaderResourceMetal(new ShaderResourceMetal(*this));
                        shaderResourceMetal->init(initShaderCommand->fragmentShader,
                                                  initShaderCommand->vertexShader,
                                                  initShaderCommand->vertexAttributes,
                                                  initShaderCommand->fragmentShaderConstantInfo,
                                                  initShaderCommand->vertexShaderConstantInfo,
                                                  initShaderCommand->fragmentShaderDataAlignment,
                                                  initShaderCommand->vertexShaderDataAlignment,
                                                  initShaderCommand->fragmentShaderFunction,
                                                  initShaderCommand->vertexShaderFunction);

                        if (initShaderCommand->shader > resources.size())
                            resources.resize(initShaderCommand->shader);
                        resources[initShaderCommand->shader - 1] = std::move(shaderResourceMetal);
                        break;
                    }

                    case Command::Type::DELETE_SHADER:
                    {
                        const DeleteShaderCommand* deleteShaderCommand = static_cast<const DeleteShaderCommand*>(command.get());
                        resources[deleteShaderCommand->shader - 1].reset();
                        break;
                    }

                    case Command::Type::SET_SHADER_CONSTANTS:
                    {
                        const SetShaderConstantsCommand* setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        if (!currentShader)
                            throw DataError("No shader set");

                        // pixel shader constants
                        const std::vector<ShaderResourceMetal::Location>& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                        if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                            throw DataError("Invalid pixel shader constant size");

                        shaderData.clear();

                        for (size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                        {
                            const ShaderResourceMetal::Location& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                            const std::vector<float>& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                            if (sizeof(float) * fragmentShaderConstant.size() != fragmentShaderConstantLocation.size)
                                throw DataError("Invalid pixel shader constant size");

                            shaderData.insert(shaderData.end(), fragmentShaderConstant.begin(), fragmentShaderConstant.end());
                        }

                        shaderConstantBuffer.offset = ((shaderConstantBuffer.offset + currentShader->getFragmentShaderAlignment() - 1) /
                                                       currentShader->getFragmentShaderAlignment()) * currentShader->getFragmentShaderAlignment(); // round up to nearest aligned pointer


                        if (shaderConstantBuffer.offset + getVectorSize(shaderData) > BUFFER_SIZE)
                        {
                            ++shaderConstantBuffer.index;
                            shaderConstantBuffer.offset = 0;
                        }

                        if (shaderConstantBuffer.index >= shaderConstantBuffer.buffers.size())
                        {
                            MTLBufferPtr buffer = [device newBufferWithLength:BUFFER_SIZE
                                                                      options:MTLResourceCPUCacheModeWriteCombined];

                            if (!buffer)
                                throw SystemError("Failed to create Metal buffer");

                            shaderConstantBuffer.buffers.push_back(buffer);
                        }

                        MTLBufferPtr currentBuffer = shaderConstantBuffer.buffers[shaderConstantBuffer.index];

                        std::copy(reinterpret_cast<const char*>(shaderData.data()),
                                  reinterpret_cast<const char*>(shaderData.data()) + static_cast<uint32_t>(sizeof(float) * shaderData.size()),
                                  static_cast<char*>([currentBuffer contents]) + shaderConstantBuffer.offset);

                        [currentRenderCommandEncoder setFragmentBuffer:currentBuffer
                                                                offset:shaderConstantBuffer.offset
                                                               atIndex:1];

                        shaderConstantBuffer.offset += static_cast<uint32_t>(getVectorSize(shaderData));

                        // vertex shader constants
                        const std::vector<ShaderResourceMetal::Location>& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                        if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                            throw DataError("Invalid vertex shader constant size");

                        shaderData.clear();

                        for (size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                        {
                            const ShaderResourceMetal::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                            const std::vector<float>& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                            if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                                throw DataError("Invalid vertex shader constant size");

                            shaderData.insert(shaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                        }

                        shaderConstantBuffer.offset = ((shaderConstantBuffer.offset + currentShader->getVertexShaderAlignment() - 1) /
                                                       currentShader->getVertexShaderAlignment()) * currentShader->getVertexShaderAlignment(); // round up to nearest aligned pointer

                        if (shaderConstantBuffer.offset + getVectorSize(shaderData) > BUFFER_SIZE)
                        {
                            ++shaderConstantBuffer.index;
                            shaderConstantBuffer.offset = 0;
                        }

                        if (shaderConstantBuffer.index >= shaderConstantBuffer.buffers.size())
                        {
                            MTLBufferPtr buffer = [device newBufferWithLength:BUFFER_SIZE
                                                                      options:MTLResourceCPUCacheModeWriteCombined];

                            if (!buffer)
                                throw SystemError("Failed to create Metal buffer");

                            shaderConstantBuffer.buffers.push_back(buffer);
                        }

                        currentBuffer = shaderConstantBuffer.buffers[shaderConstantBuffer.index];

                        std::copy(reinterpret_cast<const char*>(shaderData.data()),
                                  reinterpret_cast<const char*>(shaderData.data()) + static_cast<uint32_t>(sizeof(float) * shaderData.size()),
                                  static_cast<char*>([currentBuffer contents]) + shaderConstantBuffer.offset);

                        [currentRenderCommandEncoder setVertexBuffer:currentBuffer
                                                              offset:shaderConstantBuffer.offset
                                                             atIndex:1];

                        shaderConstantBuffer.offset += static_cast<uint32_t>(getVectorSize(shaderData));

                        break;
                    }

                    case Command::Type::INIT_TEXTURE:
                    {
                        const InitTextureCommand* initTextureCommand = static_cast<const InitTextureCommand*>(command.get());

                        std::unique_ptr<TextureResourceMetal> textureResourceMetal(new TextureResourceMetal(*this));
                        textureResourceMetal->init(initTextureCommand->levels,
                                                   initTextureCommand->flags,
                                                   initTextureCommand->sampleCount,
                                                   initTextureCommand->pixelFormat);

                        if (initTextureCommand->texture > resources.size())
                            resources.resize(initTextureCommand->texture);
                        resources[initTextureCommand->texture - 1] = std::move(textureResourceMetal);
                        break;
                    }

                    case Command::Type::DELETE_TEXTURE:
                    {
                        const DeleteTextureCommand* deleteTextureCommand = static_cast<const DeleteTextureCommand*>(command.get());
                        resources[deleteTextureCommand->texture - 1].reset();
                        break;
                    }

                    case Command::Type::SET_TEXTURE_DATA:
                    {
                        const SetTextureDataCommand* setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                        TextureResourceMetal* textureResourceMetal = static_cast<TextureResourceMetal*>(resources[setTextureDataCommand->texture - 1].get());
                        textureResourceMetal->setData(setTextureDataCommand->levels);

                        break;
                    }

                    case Command::Type::SET_TEXTURE_PARAMETERS:
                    {
                        const SetTextureParametersCommand* setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                        TextureResourceMetal* textureResourceMetal = static_cast<TextureResourceMetal*>(resources[setTextureParametersCommand->texture - 1].get());
                        textureResourceMetal->setFilter(setTextureParametersCommand->filter);
                        textureResourceMetal->setAddressX(setTextureParametersCommand->addressX);
                        textureResourceMetal->setAddressY(setTextureParametersCommand->addressY);
                        textureResourceMetal->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy);

                        break;
                    }

                    case Command::Type::SET_TEXTURES:
                    {
                        const SetTexturesCommand* setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw DataError("Metal render command encoder not initialized");

                        for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                        {
                            if (setTexturesCommand->textures[layer])
                            {
                                TextureResourceMetal* textureMetal = static_cast<TextureResourceMetal*>(resources[setTexturesCommand->textures[layer] - 1].get());
                                [currentRenderCommandEncoder setFragmentTexture:textureMetal->getTexture() atIndex:layer];
                                [currentRenderCommandEncoder setFragmentSamplerState:textureMetal->getSamplerState() atIndex:layer];
                            }
                            else
                            {
                                [currentRenderCommandEncoder setFragmentTexture:nil atIndex:layer];
                                [currentRenderCommandEncoder setFragmentSamplerState:nil atIndex:layer];
                            }
                        }

                        break;
                    }

                    default: throw DataError("Invalid command");
                }

                if (command->type == Command::Type::PRESENT) break;
            }
        }

        void RenderDeviceMetal::generateScreenshot(const std::string& filename)
        {
            if (!currentMetalTexture)
                throw SystemError("No back buffer");

            NSUInteger width = static_cast<NSUInteger>(currentMetalTexture.width);
            NSUInteger height = static_cast<NSUInteger>(currentMetalTexture.height);

            std::shared_ptr<uint8_t> data(new uint8_t[width * height * 4]);
            [currentMetalTexture getBytes:data.get() bytesPerRow:width * 4 fromRegion:MTLRegionMake2D(0, 0, width, height) mipmapLevel:0];

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
                throw FileError("Failed to save image to file");
        }

        MTLRenderPipelineStatePtr RenderDeviceMetal::getPipelineState(const PipelineStateDesc& desc)
        {
            if (!desc.blendState || !desc.shader || !desc.sampleCount) return nil;

            auto pipelineStateIterator = pipelineStates.find(desc);

            if (pipelineStateIterator != pipelineStates.end())
                return pipelineStateIterator->second;
            else
            {
                MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
                pipelineStateDescriptor.sampleCount = desc.sampleCount;

                if (desc.shader)
                {
                    assert(desc.shader->getFragmentShader());
                    assert(desc.shader->getVertexShader());
                    assert(desc.shader->getVertexDescriptor());

                    pipelineStateDescriptor.vertexFunction = desc.shader->getVertexShader();
                    pipelineStateDescriptor.fragmentFunction = desc.shader->getFragmentShader();
                    pipelineStateDescriptor.vertexDescriptor = desc.shader->getVertexDescriptor();
                }

                pipelineStateDescriptor.colorAttachments[0].pixelFormat = desc.colorFormat;
                pipelineStateDescriptor.depthAttachmentPixelFormat = desc.depthFormat;
                pipelineStateDescriptor.stencilAttachmentPixelFormat = MTLPixelFormatInvalid;

                if (desc.blendState)
                {
                    // blending
                    pipelineStateDescriptor.colorAttachments[0].blendingEnabled = desc.blendState->isMetalBlendingEnabled() ? YES : NO;

                    pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = desc.blendState->getSourceRGBBlendFactor();
                    pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = desc.blendState->getDestinationRGBBlendFactor();
                    pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = desc.blendState->getRGBBlendOperation();

                    pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = desc.blendState->getSourceAlphaBlendFactor();
                    pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = desc.blendState->getDestinationAlphaBlendFactor();
                    pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = desc.blendState->getAlphaBlendOperation();

                    pipelineStateDescriptor.colorAttachments[0].writeMask = desc.blendState->getColorWriteMask();
                }

                NSError* error;
                id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
                [pipelineStateDescriptor release];
                if (error || !pipelineState)
                {
                    if (pipelineState) [pipelineState release];
                    throw SystemError("Failed to created Metal pipeline state");
                }

                pipelineStates[desc] = pipelineState;

                return pipelineState;
            }
        }

        MTLSamplerStatePtr RenderDeviceMetal::getSamplerState(const SamplerStateDescriptor& descriptor)
        {
            auto samplerStatesIterator = samplerStates.find(descriptor);

            if (samplerStatesIterator != samplerStates.end())
                return samplerStatesIterator->second;
            else
            {
                MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
                switch (descriptor.filter)
                {
                    case Texture::Filter::DEFAULT:
                    case Texture::Filter::POINT:
                        samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
                        samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
                        samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                        break;
                    case Texture::Filter::LINEAR:
                        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                        samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
                        samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                        break;
                    case Texture::Filter::BILINEAR:
                        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
                        samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                        break;
                    case Texture::Filter::TRILINEAR:
                        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
                        samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
                        break;
                }

                switch (descriptor.addressX)
                {
                    case Texture::Address::CLAMP:
                        samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
                        break;
                    case Texture::Address::REPEAT:
                        samplerDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
                        break;
                    case Texture::Address::MIRROR_REPEAT:
                        samplerDescriptor.sAddressMode = MTLSamplerAddressModeMirrorRepeat;
                        break;
                }

                switch (descriptor.addressY)
                {
                    case Texture::Address::CLAMP:
                        samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;
                        break;
                    case Texture::Address::REPEAT:
                        samplerDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;
                        break;
                    case Texture::Address::MIRROR_REPEAT:
                        samplerDescriptor.tAddressMode = MTLSamplerAddressModeMirrorRepeat;
                        break;
                }

                samplerDescriptor.maxAnisotropy = descriptor.maxAnisotropy;

                MTLSamplerStatePtr samplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];
                [samplerDescriptor release];

                if (!samplerState)
                    throw SystemError("Failed to create Metal sampler state");

                samplerStates[descriptor] = samplerState;

                return samplerState;
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
