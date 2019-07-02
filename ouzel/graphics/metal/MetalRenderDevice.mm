// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <Availability.h>
#include <TargetConditionals.h>
#include <cassert>
#include <stdexcept>
#include "MetalRenderDevice.hpp"
#include "MetalBlendState.hpp"
#include "MetalBuffer.hpp"
#include "MetalDepthStencilState.hpp"
#include "MetalRenderTarget.hpp"
#include "MetalShader.hpp"
#include "MetalTexture.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"
#include "stb_image_write.h"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            constexpr MTLIndexType getIndexType(uint32_t indexSize)
            {
                switch (indexSize)
                {
                    case 2: return MTLIndexTypeUInt16;
                    case 4: return MTLIndexTypeUInt32;
                    default: throw std::runtime_error("Invalid index size");
                }
            }

            constexpr MTLPrimitiveType getPrimitiveType(DrawMode drawMode)
            {
                switch (drawMode)
                {
                    case DrawMode::POINT_LIST: return MTLPrimitiveTypePoint;
                    case DrawMode::LINE_LIST: return MTLPrimitiveTypeLine;
                    case DrawMode::LINE_STRIP: return MTLPrimitiveTypeLineStrip;
                    case DrawMode::TRIANGLE_LIST: return MTLPrimitiveTypeTriangle;
                    case DrawMode::TRIANGLE_STRIP: return MTLPrimitiveTypeTriangleStrip;
                    default: throw std::runtime_error("Invalid draw mode");
                }
            }

            constexpr MTLCullMode getCullMode(CullMode cullMode)
            {
                switch (cullMode)
                {
                    case CullMode::NONE: return MTLCullModeNone;
                    case CullMode::FRONT: return MTLCullModeFront;
                    case CullMode::BACK: return MTLCullModeBack;
                    default: throw std::runtime_error("Invalid cull mode");
                }
            }

            constexpr MTLTriangleFillMode getFillMode(FillMode fillMode)
            {
                switch (fillMode)
                {
                    case FillMode::SOLID: return MTLTriangleFillModeFill;
                    case FillMode::WIREFRAME: return MTLTriangleFillModeLines;
                    default: throw std::runtime_error("Invalid fill mode");
                }
            }

            constexpr MTLSamplerAddressMode getSamplerAddressMode(SamplerAddressMode address)
            {
                switch (address)
                {
                    case SamplerAddressMode::CLAMP_TO_EDGE:
                        return MTLSamplerAddressModeClampToEdge;
#if defined(__MAC_10_12) && __MAC_OS_X_VERSION_MAX_ALLOWED >= __MAC_10_12
                    case SamplerAddressMode::CLAMP_TO_BORDER:
                        return MTLSamplerAddressModeClampToBorderColor;
#endif
                    case SamplerAddressMode::REPEAT:
                        return MTLSamplerAddressModeRepeat;
                    case SamplerAddressMode::MIRROR_REPEAT:
                        return MTLSamplerAddressModeMirrorRepeat;
                    default:
                        throw std::runtime_error("Invalid address mode");
                }
            }

            bool RenderDevice::available()
            {
                id<MTLDevice> device = MTLCreateSystemDefaultDevice();

                if (device)
                {
                    [device release];
                    return true;
                }

                return false;
            }

            RenderDevice::RenderDevice(const std::function<void(const Event&)>& initCallback):
                graphics::RenderDevice(Driver::METAL, initCallback),
                colorFormat(MTLPixelFormatInvalid),
                depthFormat(MTLPixelFormatInvalid),
                stencilFormat(MTLPixelFormatInvalid)
            {
                apiMajorVersion = 1;
                apiMinorVersion = 0;
            }

            RenderDevice::~RenderDevice()
            {
                resources.clear();

                for (const ShaderConstantBuffer& shaderConstantBuffer : shaderConstantBuffers)
                    for (MTLBufferPtr buffer : shaderConstantBuffer.buffers)
                        [buffer release];

                for (const auto& samplerState : samplerStates)
                    [samplerState.second release];

                if (depthTexture) [depthTexture release];

                if (msaaTexture) [msaaTexture release];

                for (const auto& pipelineState : pipelineStates)
                    [pipelineState.second release];

                if (metalCommandQueue) [metalCommandQueue release];

                if (defaultDepthStencilState) [defaultDepthStencilState release];

                if (renderPassDescriptor) [renderPassDescriptor release];

                if (device) [device release];

                if (currentMetalTexture) [currentMetalTexture release];
            }

            void RenderDevice::init(Window* newWindow,
                                         const Size2U& newSize,
                                         uint32_t newSampleCount,
                                         SamplerFilter newTextureFilter,
                                         uint32_t newMaxAnisotropy,
                                         bool newSrgb,
                                         bool newVerticalSync,
                                         bool newDepth,
                                         bool newStencil,
                                         bool newDebugRenderer)
            {
                graphics::RenderDevice::init(newWindow,
                                                    newSize,
                                                    newSampleCount,
                                                    newTextureFilter,
                                                    newMaxAnisotropy,
                                                    newSrgb,
                                                    newVerticalSync,
                                                    newDepth,
                                                    newStencil,
                                                    newDebugRenderer);

                npotTexturesSupported = true;
                anisotropicFilteringSupported = true;
                renderTargetsSupported = true;
                multisamplingSupported = true;

                inflightSemaphore = dispatch_semaphore_create(BUFFER_COUNT);

                device = MTLCreateSystemDefaultDevice();

                if (!device)
                    throw std::runtime_error("Failed to create Metal device");

                if (device.name)
                    engine->log(Log::Level::INFO) << "Using " << [device.name cStringUsingEncoding:NSUTF8StringEncoding] << " for rendering";

#if defined(__MAC_10_12) && __MAC_OS_X_VERSION_MAX_ALLOWED >= __MAC_10_12
                if ([device supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v2])
                    clampToBorderSupported = true;
#endif

                metalCommandQueue = [device newCommandQueue];

                if (!metalCommandQueue)
                    throw std::runtime_error("Failed to create Metal command queue");

                if (depth)
                {
#if TARGET_OS_IOS || TARGET_OS_TV
                    depthFormat = stencil ? MTLPixelFormatDepth32Float_Stencil8 : MTLPixelFormatDepth32Float;
                    stencilFormat = stencil ? MTLPixelFormatDepth32Float_Stencil8 : MTLPixelFormatInvalid;
#else
                    depthFormat = stencil ? MTLPixelFormatDepth24Unorm_Stencil8 : MTLPixelFormatDepth32Float;
                    stencilFormat = stencil ? MTLPixelFormatDepth24Unorm_Stencil8 : MTLPixelFormatInvalid;
#endif
                }

                renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                if (!renderPassDescriptor)
                    throw std::runtime_error("Failed to create Metal render pass descriptor");

                renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0F, 0.0F, 0.0F, 0.0F);
                renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
                renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
                renderPassDescriptor.depthAttachment.clearDepth = 1.0F;
                renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
                renderPassDescriptor.stencilAttachment.loadAction = MTLLoadActionClear;
                renderPassDescriptor.stencilAttachment.clearStencil = 0;

                MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];

                depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionAlways; // depth read
                depthStencilDescriptor.depthWriteEnabled = NO; // depth write
                defaultDepthStencilState = [device newDepthStencilStateWithDescriptor:depthStencilDescriptor];
                [depthStencilDescriptor release];
            }

            void RenderDevice::process()
            {
                graphics::RenderDevice::process();
                executeAll();

                id<CAMetalDrawable> currentMetalDrawable = [metalLayer nextDrawable];

                if (!currentMetalDrawable)
                    throw std::runtime_error("Failed to get Metal drawable");

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

                        MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                                                        width:frameBufferWidth
                                                                                                       height:frameBufferHeight
                                                                                                    mipmapped:NO];
                        desc.textureType = MTLTextureType2DMultisample;
                        desc.storageMode = MTLStorageModePrivate;
                        desc.sampleCount = sampleCount;
                        desc.usage = MTLTextureUsageRenderTarget;

                        msaaTexture = [device newTextureWithDescriptor:desc];

                        if (!msaaTexture)
                            throw std::runtime_error("Failed to create MSAA texture");

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
                            throw std::runtime_error("Failed to create depth texture");

                        renderPassDescriptor.depthAttachment.texture = depthTexture;
                    }
                }
                else
                    renderPassDescriptor.depthAttachment.texture = nil;

                dispatch_semaphore_wait(inflightSemaphore, DISPATCH_TIME_FOREVER);

                id<MTLCommandBuffer> currentCommandBuffer = [metalCommandQueue commandBuffer];

                if (!currentCommandBuffer)
                    throw std::runtime_error("Failed to create Metal command buffer");

                __block dispatch_semaphore_t blockSemaphore = inflightSemaphore;
                [currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer>)
                 {
                     dispatch_semaphore_signal(blockSemaphore);
                 }];

                MTLRenderPassDescriptorPtr currentRenderPassDescriptor = nil;
                id<MTLRenderCommandEncoder> currentRenderCommandEncoder = nil;
                PipelineStateDesc currentPipelineStateDesc;
                std::vector<float> shaderData;

                if (++shaderConstantBufferIndex >= BUFFER_COUNT) shaderConstantBufferIndex = 0;
                ShaderConstantBuffer& shaderConstantBuffer = shaderConstantBuffers[shaderConstantBufferIndex];
                shaderConstantBuffer.index = 0;
                shaderConstantBuffer.offset = 0;
                RenderTarget* currentRenderTarget = nullptr;
                Shader* currentShader = nullptr;

                CommandBuffer commandBuffer;
                std::unique_ptr<Command> command;

                for (;;)
                {
                    std::unique_lock<std::mutex> lock(commandQueueMutex);
                    while (commandQueue.empty()) commandQueueCondition.wait(lock);
                    commandBuffer = std::move(commandQueue.front());
                    commandQueue.pop();
                    lock.unlock();

                    while (!commandBuffer.isEmpty())
                    {
                        command = commandBuffer.popCommand();

                        switch (command->type)
                        {
                            case Command::Type::RESIZE:
                            {
                                auto resizeCommand = static_cast<const ResizeCommand*>(command.get());
                                CGSize drawableSize = CGSizeMake(resizeCommand->size.v[0],
                                                                 resizeCommand->size.v[1]);
                                metalLayer.drawableSize = drawableSize;
                                break;
                            }

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

                            case Command::Type::DELETE_RESOURCE:
                            {
                                auto deleteResourceCommand = static_cast<const DeleteResourceCommand*>(command.get());
                                resources[deleteResourceCommand->resource - 1].reset();
                                break;
                            }

                            case Command::INIT_RENDER_TARGET:
                            {
                                auto initRenderTargetCommand = static_cast<const InitRenderTargetCommand*>(command.get());

                                std::set<Texture*> colorTextures;
                                for (uintptr_t colorTextureId : initRenderTargetCommand->colorTextures)
                                    colorTextures.insert(getResource<Texture>(colorTextureId));

                                auto renderTarget = std::make_unique<RenderTarget>(*this,
                                                                                   colorTextures,
                                                                                   getResource<Texture>(initRenderTargetCommand->depthTexture));

                                if (initRenderTargetCommand->renderTarget > resources.size())
                                    resources.resize(initRenderTargetCommand->renderTarget);
                                resources[initRenderTargetCommand->renderTarget - 1] = std::move(renderTarget);
                                break;
                            }

                            case Command::Type::SET_RENDER_TARGET:
                            {
                                auto setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                                MTLRenderPassDescriptorPtr newRenderPassDescriptor;

                                if (setRenderTargetCommand->renderTarget)
                                {
                                    currentRenderTarget = getResource<RenderTarget>(setRenderTargetCommand->renderTarget);

                                    newRenderPassDescriptor = currentRenderTarget->getRenderPassDescriptor();
                                    if (!newRenderPassDescriptor) break;

                                    currentPipelineStateDesc.sampleCount = currentRenderTarget->getSampleCount();
                                    currentPipelineStateDesc.colorFormats = currentRenderTarget->getColorFormats();
                                    currentPipelineStateDesc.depthFormat = currentRenderTarget->getDepthFormat();
                                    currentPipelineStateDesc.stencilFormat = currentRenderTarget->getStencilFormat();
                                }
                                else
                                {
                                    currentRenderTarget = nullptr;
                                    newRenderPassDescriptor = renderPassDescriptor;
                                    currentPipelineStateDesc.sampleCount = sampleCount;
                                    currentPipelineStateDesc.colorFormats = {colorFormat};
                                    currentPipelineStateDesc.depthFormat = depthFormat;
                                    currentPipelineStateDesc.stencilFormat = stencilFormat;
                                }

                                if (currentRenderPassDescriptor != newRenderPassDescriptor ||
                                    !currentRenderCommandEncoder)
                                {
                                    if (currentRenderCommandEncoder)
                                        [currentRenderCommandEncoder endEncoding];

                                    currentRenderPassDescriptor = newRenderPassDescriptor;
                                    currentRenderCommandEncoder = [currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor];

                                    if (!currentRenderCommandEncoder)
                                        throw std::runtime_error("Failed to create Metal render command encoder");

                                    currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
                                    currentRenderPassDescriptor.depthAttachment.loadAction = MTLLoadActionLoad;
                                }
                                break;
                            }

                            case Command::Type::CLEAR_RENDER_TARGET:
                            {
                                auto clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

                                if (currentRenderCommandEncoder)
                                    [currentRenderCommandEncoder endEncoding];

                                size_t colorAttachments = 1;
                                if (currentRenderTarget)
                                    colorAttachments = currentRenderTarget->getColorTextures().size();

                                for (size_t i = 0; i < colorAttachments; ++i)
                                {
                                    currentRenderPassDescriptor.colorAttachments[i].loadAction = clearCommand->clearColorBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
                                    currentRenderPassDescriptor.colorAttachments[i].clearColor = MTLClearColorMake(clearCommand->clearColor.normR(),
                                                                                                                   clearCommand->clearColor.normG(),
                                                                                                                   clearCommand->clearColor.normB(),
                                                                                                                   clearCommand->clearColor.normA());
                                }

                                currentRenderPassDescriptor.depthAttachment.loadAction = clearCommand->clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
                                currentRenderPassDescriptor.depthAttachment.clearDepth = clearCommand->clearDepth;

                                currentRenderPassDescriptor.stencilAttachment.loadAction = clearCommand->clearStencilBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
                                currentRenderPassDescriptor.stencilAttachment.clearStencil = clearCommand->clearStencil;

                                currentRenderCommandEncoder = [currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor];

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Failed to create Metal render command encoder");

                                // TODO: enable depth and stencil writing

                                break;
                            }

                            case Command::Type::BLIT:
                            {
                                //auto blitCommand = static_cast<const BlitCommand*>(command.get());
                                //MTLBlitCommandEncoder
                                break;
                            }

                            case Command::Type::SET_SCISSOR_TEST:
                            {
                                auto setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command.get());

                                // create a new render command encoder to set up a new scissor rect
                                if (currentRenderCommandEncoder)
                                    [currentRenderCommandEncoder endEncoding];
                                currentRenderCommandEncoder = [currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor];

                                MTLScissorRect scissorRect;

                                if (setScissorTestCommand->enabled)
                                {
                                    scissorRect.x = static_cast<NSUInteger>(setScissorTestCommand->rectangle.position.v[0]);
                                    scissorRect.y = static_cast<NSUInteger>(setScissorTestCommand->rectangle.position.v[1]);
                                    scissorRect.width = static_cast<NSUInteger>(setScissorTestCommand->rectangle.size.v[0]);
                                    scissorRect.height = static_cast<NSUInteger>(setScissorTestCommand->rectangle.size.v[1]);
                                    [currentRenderCommandEncoder setScissorRect:scissorRect];
                                }
                                break;
                            }

                            case Command::Type::SET_VIEWPORT:
                            {
                                auto setViewportCommand = static_cast<const SetViewportCommand*>(command.get());

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Metal render command encoder not initialized");

                                MTLViewport viewport;
                                viewport.originX = static_cast<double>(setViewportCommand->viewport.position.v[0]);
                                viewport.originY = static_cast<double>(setViewportCommand->viewport.position.v[1]);
                                viewport.width = static_cast<double>(setViewportCommand->viewport.size.v[0]);
                                viewport.height = static_cast<double>(setViewportCommand->viewport.size.v[1]);
                                viewport.znear = 0.0f;
                                viewport.zfar = 1.0f;

                                [currentRenderCommandEncoder setViewport:viewport];

                                break;
                            }

                            case Command::Type::INIT_DEPTH_STENCIL_STATE:
                            {
                                auto initDepthStencilStateCommand = static_cast<const InitDepthStencilStateCommand*>(command.get());
                                auto depthStencilState = std::make_unique<DepthStencilState>(*this,
                                                                                             initDepthStencilStateCommand->depthTest,
                                                                                             initDepthStencilStateCommand->depthWrite,
                                                                                             initDepthStencilStateCommand->compareFunction,
                                                                                             initDepthStencilStateCommand->stencilEnabled,
                                                                                             initDepthStencilStateCommand->stencilReadMask,
                                                                                             initDepthStencilStateCommand->stencilWriteMask,
                                                                                             initDepthStencilStateCommand->frontFaceStencilFailureOperation,
                                                                                             initDepthStencilStateCommand->frontFaceStencilDepthFailureOperation,
                                                                                             initDepthStencilStateCommand->frontFaceStencilPassOperation,
                                                                                             initDepthStencilStateCommand->frontFaceStencilCompareFunction,
                                                                                             initDepthStencilStateCommand->backFaceStencilFailureOperation,
                                                                                             initDepthStencilStateCommand->backFaceStencilDepthFailureOperation,
                                                                                             initDepthStencilStateCommand->backFaceStencilPassOperation,
                                                                                             initDepthStencilStateCommand->backFaceStencilCompareFunction);

                                if (initDepthStencilStateCommand->depthStencilState > resources.size())
                                    resources.resize(initDepthStencilStateCommand->depthStencilState);
                                resources[initDepthStencilStateCommand->depthStencilState - 1] = std::move(depthStencilState);

                                break;
                            }

                            case Command::Type::SET_DEPTH_STENCIL_STATE:
                            {
                                auto setDepthStencilStateCommand = static_cast<const SetDepthStencilStateCommand*>(command.get());

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Metal render command encoder not initialized");

                                if (setDepthStencilStateCommand->depthStencilState)
                                {
                                    DepthStencilState* depthStencilState = getResource<DepthStencilState>(setDepthStencilStateCommand->depthStencilState);
                                    [currentRenderCommandEncoder setDepthStencilState:depthStencilState->getDepthStencilState()];
                                }
                                else
                                    [currentRenderCommandEncoder setDepthStencilState:defaultDepthStencilState];

                                [currentRenderCommandEncoder setStencilFrontReferenceValue:setDepthStencilStateCommand->stencilReferenceValue
                                                                        backReferenceValue:setDepthStencilStateCommand->stencilReferenceValue];

                                break;
                            }

                            case Command::Type::SET_PIPELINE_STATE:
                            {
                                auto setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Metal render command encoder not initialized");

                                BlendState* blendState = getResource<BlendState>(setPipelineStateCommand->blendState);
                                Shader* shader = getResource<Shader>(setPipelineStateCommand->shader);
                                currentShader = shader;

                                currentPipelineStateDesc.blendState = blendState;
                                currentPipelineStateDesc.shader = shader;

                                MTLRenderPipelineStatePtr pipelineState = getPipelineState(currentPipelineStateDesc);
                                if (pipelineState) [currentRenderCommandEncoder setRenderPipelineState:pipelineState];

                                [currentRenderCommandEncoder setCullMode:getCullMode(setPipelineStateCommand->cullMode)];
                                [currentRenderCommandEncoder setTriangleFillMode:getFillMode(setPipelineStateCommand->fillMode)];

                                break;
                            }

                            case Command::Type::DRAW:
                            {
                                auto drawCommand = static_cast<const DrawCommand*>(command.get());

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Metal render command encoder not initialized");

                                // mesh buffer
                                Buffer* indexBuffer = getResource<Buffer>(drawCommand->indexBuffer);
                                Buffer* vertexBuffer = getResource<Buffer>(drawCommand->vertexBuffer);

                                assert(indexBuffer);
                                assert(indexBuffer->getBuffer());
                                assert(vertexBuffer);
                                assert(vertexBuffer->getBuffer());

                                [currentRenderCommandEncoder setVertexBuffer:vertexBuffer->getBuffer() offset:0 atIndex:0];

                                // draw
                                assert(drawCommand->indexCount);
                                assert(indexBuffer->getSize());
                                assert(vertexBuffer->getSize());

                                [currentRenderCommandEncoder drawIndexedPrimitives:getPrimitiveType(drawCommand->drawMode)
                                                                        indexCount:drawCommand->indexCount
                                                                         indexType:getIndexType(drawCommand->indexSize)
                                                                       indexBuffer:indexBuffer->getBuffer()
                                                                 indexBufferOffset:drawCommand->startIndex * drawCommand->indexSize];

                                break;
                            }

                            case Command::Type::PUSH_DEBUG_MARKER:
                            {
                                auto pushDebugMarkerCommand = static_cast<const PushDebugMarkerCommand*>(command.get());

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Metal render command encoder not initialized");

                                [currentRenderCommandEncoder pushDebugGroup:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:pushDebugMarkerCommand->name.c_str()])];
                                break;
                            }

                            case Command::Type::POP_DEBUG_MARKER:
                            {
                                //auto popDebugMarkerCommand = static_cast<const PopDebugMarkerCommand*>(command);

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Metal render command encoder not initialized");

                                [currentRenderCommandEncoder popDebugGroup];
                                break;
                            }

                            case Command::Type::INIT_BLEND_STATE:
                            {
                                auto initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

                                auto blendState = std::make_unique<BlendState>(*this,
                                                                               initBlendStateCommand->enableBlending,
                                                                               initBlendStateCommand->colorBlendSource,
                                                                               initBlendStateCommand->colorBlendDest,
                                                                               initBlendStateCommand->colorOperation,
                                                                               initBlendStateCommand->alphaBlendSource,
                                                                               initBlendStateCommand->alphaBlendDest,
                                                                               initBlendStateCommand->alphaOperation,
                                                                               initBlendStateCommand->colorMask);

                                if (initBlendStateCommand->blendState > resources.size())
                                    resources.resize(initBlendStateCommand->blendState);
                                resources[initBlendStateCommand->blendState - 1] = std::move(blendState);
                                break;
                            }

                            case Command::Type::INIT_BUFFER:
                            {
                                auto initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

                                auto buffer = std::make_unique<Buffer>(*this,
                                                                        initBufferCommand->bufferType,
                                                                        initBufferCommand->flags,
                                                                        initBufferCommand->data,
                                                                        initBufferCommand->size);

                                if (initBufferCommand->buffer > resources.size())
                                    resources.resize(initBufferCommand->buffer);
                                resources[initBufferCommand->buffer - 1] = std::move(buffer);
                                break;
                            }

                            case Command::Type::SET_BUFFER_DATA:
                            {
                                auto setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                                Buffer* buffer = getResource<Buffer>(setBufferDataCommand->buffer);
                                buffer->setData(setBufferDataCommand->data);
                                break;
                            }

                            case Command::Type::INIT_SHADER:
                            {
                                auto initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                                auto shader = std::make_unique<Shader>(*this,
                                                                       initShaderCommand->fragmentShader,
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
                                resources[initShaderCommand->shader - 1] = std::move(shader);
                                break;
                            }

                            case Command::Type::SET_SHADER_CONSTANTS:
                            {
                                auto setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command.get());

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Metal render command encoder not initialized");

                                if (!currentShader)
                                    throw std::runtime_error("No shader set");

                                // pixel shader constants
                                const std::vector<Shader::Location>& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                                if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                                    throw std::runtime_error("Invalid pixel shader constant size");

                                shaderData.clear();

                                for (size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                                {
                                    const Shader::Location& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                                    const std::vector<float>& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                                    if (sizeof(float) * fragmentShaderConstant.size() != fragmentShaderConstantLocation.size)
                                        throw std::runtime_error("Invalid pixel shader constant size");

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
                                        throw std::runtime_error("Failed to create Metal buffer");

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
                                const std::vector<Shader::Location>& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                                if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                                    throw std::runtime_error("Invalid vertex shader constant size");

                                shaderData.clear();

                                for (size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                                {
                                    const Shader::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                                    const std::vector<float>& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                                    if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                                        throw std::runtime_error("Invalid vertex shader constant size");

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
                                        throw std::runtime_error("Failed to create Metal buffer");

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
                                auto initTextureCommand = static_cast<const InitTextureCommand*>(command.get());

                                auto texture = std::make_unique<Texture>(*this,
                                                                         initTextureCommand->levels,
                                                                         initTextureCommand->textureType,
                                                                         initTextureCommand->flags,
                                                                         initTextureCommand->sampleCount,
                                                                         initTextureCommand->pixelFormat);

                                if (initTextureCommand->texture > resources.size())
                                    resources.resize(initTextureCommand->texture);
                                resources[initTextureCommand->texture - 1] = std::move(texture);
                                break;
                            }

                            case Command::Type::SET_TEXTURE_DATA:
                            {
                                auto setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                                Texture* texture = getResource<Texture>(setTextureDataCommand->texture);
                                texture->setData(setTextureDataCommand->levels);

                                break;
                            }

                            case Command::Type::SET_TEXTURE_PARAMETERS:
                            {
                                auto setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                                Texture* texture = getResource<Texture>(setTextureParametersCommand->texture);
                                texture->setFilter(setTextureParametersCommand->filter == SamplerFilter::DEFAULT ? textureFilter : setTextureParametersCommand->filter);
                                texture->setAddressX(setTextureParametersCommand->addressX);
                                texture->setAddressY(setTextureParametersCommand->addressY);
                                texture->setAddressZ(setTextureParametersCommand->addressZ);
                                texture->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy == 0 ? maxAnisotropy : setTextureParametersCommand->maxAnisotropy);

                                break;
                            }

                            case Command::Type::SET_TEXTURES:
                            {
                                auto setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                                if (!currentRenderCommandEncoder)
                                    throw std::runtime_error("Metal render command encoder not initialized");

                                for (uint32_t layer = 0; layer < setTexturesCommand->textures.size(); ++layer)
                                {
                                    if (Texture* texture = getResource<Texture>(setTexturesCommand->textures[layer]))
                                    {
                                        [currentRenderCommandEncoder setFragmentTexture:texture->getTexture() atIndex:layer];
                                        [currentRenderCommandEncoder setFragmentSamplerState:texture->getSamplerState() atIndex:layer];
                                    }
                                    else
                                    {
                                        [currentRenderCommandEncoder setFragmentTexture:nil atIndex:layer];
                                        [currentRenderCommandEncoder setFragmentSamplerState:nil atIndex:layer];
                                    }
                                }

                                break;
                            }

                            default: throw std::runtime_error("Invalid command");
                        }

                        if (command->type == Command::Type::PRESENT) return;
                    }
                }
            }

            void RenderDevice::generateScreenshot(const std::string& filename)
            {
                if (!currentMetalTexture)
                    throw std::runtime_error("No back buffer");

                NSUInteger width = static_cast<NSUInteger>(currentMetalTexture.width);
                NSUInteger height = static_cast<NSUInteger>(currentMetalTexture.height);

                std::vector<uint8_t> data(width * height * 4);
                [currentMetalTexture getBytes:data.data() bytesPerRow:width * 4 fromRegion:MTLRegionMake2D(0, 0, width, height) mipmapLevel:0];

                uint8_t temp;
                for (uint32_t y = 0; y < height; ++y)
                {
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        // reverse the order of channels
                        temp = data[((y * width + x) * 4)];
                        data[((y * width + x) * 4)] = data[((y * width + x) * 4) + 2];
                        data[((y * width + x) * 4) + 2] = temp;
                        data[((y * width + x) * 4) + 3] = 255;
                    }
                }

                if (!stbi_write_png(filename.c_str(), static_cast<int>(width), static_cast<int>(height), 4, data.data(), static_cast<int>(width * 4)))
                    throw std::runtime_error("Failed to save image to file");
            }

            MTLRenderPipelineStatePtr RenderDevice::getPipelineState(const PipelineStateDesc& desc)
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

                    for (size_t i = 0; i < desc.colorFormats.size(); ++i)
                        pipelineStateDescriptor.colorAttachments[i].pixelFormat = desc.colorFormats[i];
                    pipelineStateDescriptor.depthAttachmentPixelFormat = desc.depthFormat;
                    pipelineStateDescriptor.stencilAttachmentPixelFormat = desc.stencilFormat;

                    if (desc.blendState)
                    {
                        for (size_t i = 0; i < desc.colorFormats.size(); ++i)
                        {
                            // blending
                            pipelineStateDescriptor.colorAttachments[i].blendingEnabled = desc.blendState->isBlendingEnabled() ? YES : NO;

                            pipelineStateDescriptor.colorAttachments[i].sourceRGBBlendFactor = desc.blendState->getSourceRgbBlendFactor();
                            pipelineStateDescriptor.colorAttachments[i].destinationRGBBlendFactor = desc.blendState->getDestinationRgbBlendFactor();
                            pipelineStateDescriptor.colorAttachments[i].rgbBlendOperation = desc.blendState->getRgbBlendOperation();

                            pipelineStateDescriptor.colorAttachments[i].sourceAlphaBlendFactor = desc.blendState->getSourceAlphaBlendFactor();
                            pipelineStateDescriptor.colorAttachments[i].destinationAlphaBlendFactor = desc.blendState->getDestinationAlphaBlendFactor();
                            pipelineStateDescriptor.colorAttachments[i].alphaBlendOperation = desc.blendState->getAlphaBlendOperation();

                            pipelineStateDescriptor.colorAttachments[i].writeMask = desc.blendState->getColorWriteMask();
                        }
                    }

                    NSError* error;
                    id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
                    [pipelineStateDescriptor release];
                    if (error || !pipelineState)
                    {
                        if (pipelineState) [pipelineState release];
                        throw std::runtime_error("Failed to created Metal pipeline state");
                    }

                    pipelineStates[desc] = pipelineState;

                    return pipelineState;
                }
            }

            MTLSamplerStatePtr RenderDevice::getSamplerState(const SamplerStateDescriptor& descriptor)
            {
                auto samplerStatesIterator = samplerStates.find(descriptor);

                if (samplerStatesIterator != samplerStates.end())
                    return samplerStatesIterator->second;
                else
                {
                    MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
                    switch (descriptor.filter)
                    {
                        case SamplerFilter::DEFAULT:
                        case SamplerFilter::POINT:
                            samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
                            samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
                            samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                            break;
                        case SamplerFilter::LINEAR:
                            samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                            samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
                            samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                            break;
                        case SamplerFilter::BILINEAR:
                            samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                            samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
                            samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                            break;
                        case SamplerFilter::TRILINEAR:
                            samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                            samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
                            samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
                            break;
                        default:
                            throw std::runtime_error("Invalid texture filter");
                    }

                    samplerDescriptor.sAddressMode = getSamplerAddressMode(descriptor.addressX);
                    samplerDescriptor.tAddressMode = getSamplerAddressMode(descriptor.addressY);
                    samplerDescriptor.rAddressMode = getSamplerAddressMode(descriptor.addressZ);

                    samplerDescriptor.maxAnisotropy = descriptor.maxAnisotropy;

                    MTLSamplerStatePtr samplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];
                    [samplerDescriptor release];

                    if (!samplerState)
                        throw std::runtime_error("Failed to create Metal sampler state");

                    samplerStates[descriptor] = samplerState;

                    return samplerState;
                }
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
