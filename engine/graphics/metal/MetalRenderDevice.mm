// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <Availability.h>
#include <TargetConditionals.h>
#include <cassert>
#include <cstring>
#include "MetalRenderDevice.hpp"
#include "MetalBlendState.hpp"
#include "MetalBuffer.hpp"
#include "MetalDepthStencilState.hpp"
#include "MetalError.hpp"
#include "MetalRenderTarget.hpp"
#include "MetalShader.hpp"
#include "MetalTexture.hpp"
#include "../../core/Engine.hpp"
#include "../../events/EventDispatcher.hpp"
#include "../../utils/Log.hpp"
#include "../../utils/Utils.hpp"
#include "stb_image_write.h"

namespace ouzel::graphics::metal
{
    namespace
    {
        constexpr MTLIndexType getIndexType(std::uint32_t indexSize)
        {
            switch (indexSize)
            {
                case 2: return MTLIndexTypeUInt16;
                case 4: return MTLIndexTypeUInt32;
                default: throw Error("Invalid index size");
            }
        }

        constexpr MTLPrimitiveType getPrimitiveType(DrawMode drawMode)
        {
            switch (drawMode)
            {
                case DrawMode::pointList: return MTLPrimitiveTypePoint;
                case DrawMode::lineList: return MTLPrimitiveTypeLine;
                case DrawMode::lineStrip: return MTLPrimitiveTypeLineStrip;
                case DrawMode::triangleList: return MTLPrimitiveTypeTriangle;
                case DrawMode::triangleStrip: return MTLPrimitiveTypeTriangleStrip;
                default: throw Error("Invalid draw mode");
            }
        }

        constexpr MTLCullMode getCullMode(CullMode cullMode)
        {
            switch (cullMode)
            {
                case CullMode::none: return MTLCullModeNone;
                case CullMode::front: return MTLCullModeFront;
                case CullMode::back: return MTLCullModeBack;
                default: throw Error("Invalid cull mode");
            }
        }

        constexpr MTLTriangleFillMode getFillMode(FillMode fillMode)
        {
            switch (fillMode)
            {
                case FillMode::solid: return MTLTriangleFillModeFill;
                case FillMode::wireframe: return MTLTriangleFillModeLines;
                default: throw Error("Invalid fill mode");
            }
        }

        constexpr MTLSamplerAddressMode getSamplerAddressMode(SamplerAddressMode address)
        {
            switch (address)
            {
                case SamplerAddressMode::clampToEdge:
                    return MTLSamplerAddressModeClampToEdge;
#if defined(__MAC_10_12) && __MAC_OS_X_VERSION_MAX_ALLOWED >= __MAC_10_12
                // MTLSamplerAddressModeClampToBorderColor is not defined in macOS SDK older than 10.12
                case SamplerAddressMode::clampToBorder:
                    return MTLSamplerAddressModeClampToBorderColor;
#endif
                case SamplerAddressMode::repeat:
                    return MTLSamplerAddressModeRepeat;
                case SamplerAddressMode::mirrorRepeat:
                    return MTLSamplerAddressModeMirrorRepeat;
                default:
                    throw Error("Invalid address mode");
            }
        }
    }

    bool RenderDevice::available() noexcept
    {
        Pointer<id<MTLDevice>> device = MTLCreateSystemDefaultDevice();

        return static_cast<bool>(device);
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& newWindow):
        graphics::RenderDevice{Driver::metal, settings, newWindow},
        colorFormat{MTLPixelFormatInvalid},
        depthFormat{MTLPixelFormatInvalid},
        stencilFormat{MTLPixelFormatInvalid}
    {
        apiVersion = ApiVersion{1, 0};

        npotTexturesSupported = true;
        anisotropicFilteringSupported = true;
        renderTargetsSupported = true;
        multisamplingSupported = true;
        uintIndicesSupported = true;

        device = MTLCreateSystemDefaultDevice();

        if (!device)
            throw Error("Failed to create Metal device");

        if (device.get().name)
            logger.log(Log::Level::info) << "Using " << [device.get().name cStringUsingEncoding:NSUTF8StringEncoding] << " for rendering";

#if defined(__MAC_10_12) && __MAC_OS_X_VERSION_MAX_ALLOWED >= __MAC_10_12
        // MTLFeatureSet_macOS_GPUFamily1_v2 is not defined in macOS SDK older than 10.12
        if ([device.get() supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v2])
            clampToBorderSupported = true;
#endif

        metalCommandQueue = [device.get() newCommandQueue];

        if (!metalCommandQueue)
            throw Error("Failed to create Metal command queue");

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
            throw Error("Failed to create Metal render pass descriptor");

        renderPassDescriptor.get().colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPassDescriptor.get().colorAttachments[0].clearColor = MTLClearColorMake(0.0F, 0.0F, 0.0F, 0.0F);
        renderPassDescriptor.get().colorAttachments[0].storeAction = MTLStoreActionStore;
        renderPassDescriptor.get().depthAttachment.loadAction = MTLLoadActionClear;
        renderPassDescriptor.get().depthAttachment.clearDepth = 1.0F;
        renderPassDescriptor.get().depthAttachment.storeAction = MTLStoreActionStore;
        renderPassDescriptor.get().stencilAttachment.loadAction = MTLLoadActionClear;
        renderPassDescriptor.get().stencilAttachment.clearStencil = 0;

        Pointer<MTLDepthStencilDescriptor*> depthStencilDescriptor = [[MTLDepthStencilDescriptor alloc] init];

        depthStencilDescriptor.get().depthCompareFunction = MTLCompareFunctionAlways; // depth read
        depthStencilDescriptor.get().depthWriteEnabled = NO; // depth write
        defaultDepthStencilState = [device.get() newDepthStencilStateWithDescriptor:depthStencilDescriptor.get()];
    }

    void RenderDevice::process()
    {
        graphics::RenderDevice::process();
        executeAll();

        id<CAMetalDrawable> currentMetalDrawable = [metalLayer nextDrawable];

        if (!currentMetalDrawable)
            throw Error("Failed to get Metal drawable");

        currentMetalTexture = [currentMetalDrawable.texture retain];

        const NSUInteger frameBufferWidth = currentMetalTexture.get().width;
        const NSUInteger frameBufferHeight = currentMetalTexture.get().height;

        if (sampleCount > 1)
        {
            if (!msaaTexture ||
                frameBufferWidth != msaaTexture.get().width ||
                frameBufferHeight != msaaTexture.get().height)
            {
                MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                                                width:frameBufferWidth
                                                                                               height:frameBufferHeight
                                                                                            mipmapped:NO];
                desc.textureType = MTLTextureType2DMultisample;
                desc.storageMode = MTLStorageModePrivate;
                desc.sampleCount = sampleCount;
                desc.usage = MTLTextureUsageRenderTarget;

                msaaTexture = [device.get() newTextureWithDescriptor:desc];

                if (!msaaTexture)
                    throw Error("Failed to create MSAA texture");

                renderPassDescriptor.get().colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
                renderPassDescriptor.get().colorAttachments[0].texture = msaaTexture.get();
            }

            renderPassDescriptor.get().colorAttachments[0].resolveTexture = currentMetalTexture.get();
        }
        else
            renderPassDescriptor.get().colorAttachments[0].texture = currentMetalTexture.get();

        if (depth)
        {
            if (!depthTexture ||
                frameBufferWidth != depthTexture.get().width ||
                frameBufferHeight != depthTexture.get().height)
            {
                MTLTextureDescriptor* desc = [MTLTextureDescriptor
                                              texture2DDescriptorWithPixelFormat:static_cast<MTLPixelFormat>(depthFormat)
                                              width:frameBufferWidth height:frameBufferHeight mipmapped:NO];

                desc.textureType = (sampleCount > 1) ? MTLTextureType2DMultisample : MTLTextureType2D;
                desc.storageMode = MTLStorageModePrivate;
                desc.sampleCount = sampleCount;
                desc.usage = MTLTextureUsageRenderTarget;

                depthTexture = [device.get() newTextureWithDescriptor:desc];

                if (!depthTexture)
                    throw Error("Failed to create depth texture");

                renderPassDescriptor.get().depthAttachment.texture = depthTexture.get();
            }
        }
        else
            renderPassDescriptor.get().depthAttachment.texture = nil;

        inflightSemaphore.wait(DISPATCH_TIME_FOREVER);

        id<MTLCommandBuffer> currentCommandBuffer = [metalCommandQueue.get() commandBuffer];

        if (!currentCommandBuffer)
            throw Error("Failed to create Metal command buffer");

        __block auto& renderer = *this;
        [currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
            renderer.inflightSemaphore.signal();
         }];

        MTLRenderPassDescriptorPtr currentRenderPassDescriptor = nil;
        id<MTLRenderCommandEncoder> currentRenderCommandEncoder = nil;
        PipelineStateDesc currentPipelineStateDesc;
        std::vector<float> shaderData;

        if (++shaderConstantBufferIndex >= bufferCount) shaderConstantBufferIndex = 0;
        auto& shaderConstantBuffer = shaderConstantBuffers[shaderConstantBufferIndex];
        shaderConstantBuffer.index = 0;
        shaderConstantBuffer.offset = 0;
        const RenderTarget* currentRenderTarget = nullptr;
        const Shader* currentShader = nullptr;

        CommandBuffer commandBuffer;
        std::unique_ptr<Command> command;

        for (;;)
        {
            std::unique_lock lock(commandQueueMutex);
            commandQueueCondition.wait(lock, [this]() noexcept { return !commandQueue.empty(); });
            commandBuffer = std::move(commandQueue.front());
            commandQueue.pop();
            lock.unlock();

            while (!commandBuffer.isEmpty())
            {
                command = commandBuffer.popCommand();

                switch (command->type)
                {
                    case Command::Type::resize:
                    {
                        const auto resizeCommand = static_cast<const ResizeCommand*>(command.get());
                        const CGSize drawableSize = CGSizeMake(resizeCommand->size.v[0],
                                                               resizeCommand->size.v[1]);
                        metalLayer.drawableSize = drawableSize;
                        break;
                    }

                    case Command::Type::present:
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

                    case Command::Type::deleteResource:
                    {
                        const auto deleteResourceCommand = static_cast<const DeleteResourceCommand*>(command.get());
                        resources[deleteResourceCommand->resource - 1].reset();
                        break;
                    }

                    case Command::Type::initRenderTarget:
                    {
                        const auto initRenderTargetCommand = static_cast<const InitRenderTargetCommand*>(command.get());

                        std::set<Texture*> colorTextures;
                        for (const auto colorTextureId : initRenderTargetCommand->colorTextures)
                            colorTextures.insert(getResource<Texture>(colorTextureId));

                        auto renderTarget = std::make_unique<RenderTarget>(*this,
                                                                           colorTextures,
                                                                           getResource<Texture>(initRenderTargetCommand->depthTexture));

                        if (initRenderTargetCommand->renderTarget > resources.size())
                            resources.resize(initRenderTargetCommand->renderTarget);
                        resources[initRenderTargetCommand->renderTarget - 1] = std::move(renderTarget);
                        break;
                    }

                    case Command::Type::setRenderTarget:
                    {
                        const auto setRenderTargetCommand = static_cast<const SetRenderTargetCommand*>(command.get());

                        MTLRenderPassDescriptorPtr newRenderPassDescriptor;

                        if (setRenderTargetCommand->renderTarget)
                        {
                            currentRenderTarget = getResource<RenderTarget>(setRenderTargetCommand->renderTarget);

                            newRenderPassDescriptor = currentRenderTarget->getRenderPassDescriptor().get();
                            if (!newRenderPassDescriptor) break;

                            currentPipelineStateDesc.sampleCount = currentRenderTarget->getSampleCount();
                            currentPipelineStateDesc.colorFormats = currentRenderTarget->getColorFormats();
                            currentPipelineStateDesc.depthFormat = currentRenderTarget->getDepthFormat();
                            currentPipelineStateDesc.stencilFormat = currentRenderTarget->getStencilFormat();
                        }
                        else
                        {
                            currentRenderTarget = nullptr;
                            newRenderPassDescriptor = renderPassDescriptor.get();
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
                                throw Error("Failed to create Metal render command encoder");

                            currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
                            currentRenderPassDescriptor.depthAttachment.loadAction = MTLLoadActionLoad;
                        }
                        break;
                    }

                    case Command::Type::clearRenderTarget:
                    {
                        const auto clearCommand = static_cast<const ClearRenderTargetCommand*>(command.get());

                        if (currentRenderCommandEncoder)
                            [currentRenderCommandEncoder endEncoding];

                        std::size_t colorAttachments = 1;
                        if (currentRenderTarget)
                            colorAttachments = currentRenderTarget->getColorTextures().size();

                        for (std::size_t i = 0; i < colorAttachments; ++i)
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
                            throw Error("Failed to create Metal render command encoder");

                        // TODO: enable depth and stencil writing

                        break;
                    }

                    case Command::Type::setScissorTest:
                    {
                        const auto setScissorTestCommand = static_cast<const SetScissorTestCommand*>(command.get());

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

                    case Command::Type::setViewport:
                    {
                        const auto setViewportCommand = static_cast<const SetViewportCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw Error("Metal render command encoder not initialized");

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

                    case Command::Type::initDepthStencilState:
                    {
                        const auto initDepthStencilStateCommand = static_cast<const InitDepthStencilStateCommand*>(command.get());
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

                    case Command::Type::setDepthStencilState:
                    {
                        const auto setDepthStencilStateCommand = static_cast<const SetDepthStencilStateCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw Error("Metal render command encoder not initialized");

                        if (setDepthStencilStateCommand->depthStencilState)
                        {
                            const auto depthStencilState = getResource<DepthStencilState>(setDepthStencilStateCommand->depthStencilState);
                            [currentRenderCommandEncoder setDepthStencilState:depthStencilState->getDepthStencilState().get()];
                        }
                        else
                            [currentRenderCommandEncoder setDepthStencilState:defaultDepthStencilState.get()];

                        [currentRenderCommandEncoder setStencilFrontReferenceValue:setDepthStencilStateCommand->stencilReferenceValue
                                                                backReferenceValue:setDepthStencilStateCommand->stencilReferenceValue];

                        break;
                    }

                    case Command::Type::setPipelineState:
                    {
                        const auto setPipelineStateCommand = static_cast<const SetPipelineStateCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw Error("Metal render command encoder not initialized");

                        const auto blendState = getResource<BlendState>(setPipelineStateCommand->blendState);
                        const auto shader = getResource<Shader>(setPipelineStateCommand->shader);
                        currentShader = shader;

                        currentPipelineStateDesc.blendState = blendState;
                        currentPipelineStateDesc.shader = shader;

                        MTLRenderPipelineStatePtr pipelineState = getPipelineState(currentPipelineStateDesc);
                        if (pipelineState) [currentRenderCommandEncoder setRenderPipelineState:pipelineState];

                        [currentRenderCommandEncoder setCullMode:getCullMode(setPipelineStateCommand->cullMode)];
                        [currentRenderCommandEncoder setTriangleFillMode:getFillMode(setPipelineStateCommand->fillMode)];

                        break;
                    }

                    case Command::Type::draw:
                    {
                        const auto drawCommand = static_cast<const DrawCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw Error("Metal render command encoder not initialized");

                        // mesh buffer
                        const auto indexBuffer = getResource<Buffer>(drawCommand->indexBuffer);
                        const auto vertexBuffer = getResource<Buffer>(drawCommand->vertexBuffer);

                        assert(indexBuffer);
                        assert(indexBuffer->getBuffer());
                        assert(vertexBuffer);
                        assert(vertexBuffer->getBuffer());

                        [currentRenderCommandEncoder setVertexBuffer:vertexBuffer->getBuffer().get() offset:0 atIndex:0];

                        // draw
                        assert(drawCommand->indexCount);
                        assert(indexBuffer->getSize());
                        assert(vertexBuffer->getSize());

                        [currentRenderCommandEncoder drawIndexedPrimitives:getPrimitiveType(drawCommand->drawMode)
                                                                indexCount:drawCommand->indexCount
                                                                 indexType:getIndexType(drawCommand->indexSize)
                                                               indexBuffer:indexBuffer->getBuffer().get()
                                                         indexBufferOffset:drawCommand->startIndex * drawCommand->indexSize];

                        break;
                    }

                    case Command::Type::initBlendState:
                    {
                        const auto initBlendStateCommand = static_cast<const InitBlendStateCommand*>(command.get());

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

                    case Command::Type::initBuffer:
                    {
                        const auto initBufferCommand = static_cast<const InitBufferCommand*>(command.get());

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

                    case Command::Type::setBufferData:
                    {
                        const auto setBufferDataCommand = static_cast<const SetBufferDataCommand*>(command.get());

                        const auto buffer = getResource<Buffer>(setBufferDataCommand->buffer);
                        buffer->setData(setBufferDataCommand->data);
                        break;
                    }

                    case Command::Type::initShader:
                    {
                        const auto initShaderCommand = static_cast<const InitShaderCommand*>(command.get());

                        auto shader = std::make_unique<Shader>(*this,
                                                               initShaderCommand->fragmentShader,
                                                               initShaderCommand->vertexShader,
                                                               initShaderCommand->vertexAttributes,
                                                               initShaderCommand->fragmentShaderConstantInfo,
                                                               initShaderCommand->vertexShaderConstantInfo,
                                                               initShaderCommand->fragmentShaderFunction,
                                                               initShaderCommand->vertexShaderFunction);

                        if (initShaderCommand->shader > resources.size())
                            resources.resize(initShaderCommand->shader);
                        resources[initShaderCommand->shader - 1] = std::move(shader);
                        break;
                    }

                    case Command::Type::setShaderConstants:
                    {
                        const auto setShaderConstantsCommand = static_cast<const SetShaderConstantsCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw Error("Metal render command encoder not initialized");

                        if (!currentShader)
                            throw Error("No shader set");

                        // pixel shader constants
                        const auto& fragmentShaderConstantLocations = currentShader->getFragmentShaderConstantLocations();

                        if (setShaderConstantsCommand->fragmentShaderConstants.size() > fragmentShaderConstantLocations.size())
                            throw Error("Invalid pixel shader constant size");

                        shaderData.clear();

                        for (std::size_t i = 0; i < setShaderConstantsCommand->fragmentShaderConstants.size(); ++i)
                        {
                            const auto& fragmentShaderConstantLocation = fragmentShaderConstantLocations[i];
                            const auto& fragmentShaderConstant = setShaderConstantsCommand->fragmentShaderConstants[i];

                            if (sizeof(float) * fragmentShaderConstant.size() != fragmentShaderConstantLocation.size)
                                throw Error("Invalid pixel shader constant size");

                            shaderData.insert(shaderData.end(), fragmentShaderConstant.begin(), fragmentShaderConstant.end());
                        }

                        shaderConstantBuffer.offset = ((shaderConstantBuffer.offset + currentShader->getFragmentShaderAlignment() - 1) /
                                                       currentShader->getFragmentShaderAlignment()) * currentShader->getFragmentShaderAlignment(); // round up to nearest aligned pointer

                        if (shaderConstantBuffer.offset + getVectorSize(shaderData) > bufferSize)
                        {
                            ++shaderConstantBuffer.index;
                            shaderConstantBuffer.offset = 0;
                        }

                        if (shaderConstantBuffer.index >= shaderConstantBuffer.buffers.size())
                        {
                            MTLBufferPtr buffer = [device.get() newBufferWithLength:bufferSize
                                                                            options:MTLResourceCPUCacheModeWriteCombined];

                            if (!buffer)
                                throw Error("Failed to create Metal buffer");

                            shaderConstantBuffer.buffers.push_back(buffer);
                        }

                        MTLBufferPtr currentBuffer = shaderConstantBuffer.buffers[shaderConstantBuffer.index].get();

                        std::memcpy(static_cast<char*>([currentBuffer contents]) + shaderConstantBuffer.offset,
                                    shaderData.data(), shaderData.size() * sizeof(float));

                        [currentRenderCommandEncoder setFragmentBuffer:currentBuffer
                                                                offset:shaderConstantBuffer.offset
                                                               atIndex:1];

                        shaderConstantBuffer.offset += static_cast<std::uint32_t>(getVectorSize(shaderData));

                        // vertex shader constants
                        const auto& vertexShaderConstantLocations = currentShader->getVertexShaderConstantLocations();

                        if (setShaderConstantsCommand->vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                            throw Error("Invalid vertex shader constant size");

                        shaderData.clear();

                        for (std::size_t i = 0; i < setShaderConstantsCommand->vertexShaderConstants.size(); ++i)
                        {
                            const auto& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                            const auto& vertexShaderConstant = setShaderConstantsCommand->vertexShaderConstants[i];

                            if (sizeof(float) * vertexShaderConstant.size() != vertexShaderConstantLocation.size)
                                throw Error("Invalid vertex shader constant size");

                            shaderData.insert(shaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                        }

                        shaderConstantBuffer.offset = ((shaderConstantBuffer.offset + currentShader->getVertexShaderAlignment() - 1) /
                                                       currentShader->getVertexShaderAlignment()) * currentShader->getVertexShaderAlignment(); // round up to nearest aligned pointer

                        if (shaderConstantBuffer.offset + getVectorSize(shaderData) > bufferSize)
                        {
                            ++shaderConstantBuffer.index;
                            shaderConstantBuffer.offset = 0;
                        }

                        if (shaderConstantBuffer.index >= shaderConstantBuffer.buffers.size())
                        {
                            MTLBufferPtr buffer = [device.get() newBufferWithLength:bufferSize
                                                                            options:MTLResourceCPUCacheModeWriteCombined];

                            if (!buffer)
                                throw Error("Failed to create Metal buffer");

                            shaderConstantBuffer.buffers.push_back(buffer);
                        }

                        currentBuffer = shaderConstantBuffer.buffers[shaderConstantBuffer.index].get();

                        std::memcpy(static_cast<char*>([currentBuffer contents]) + shaderConstantBuffer.offset,
                                    shaderData.data(), shaderData.size() * sizeof(float));

                        [currentRenderCommandEncoder setVertexBuffer:currentBuffer
                                                              offset:shaderConstantBuffer.offset
                                                             atIndex:1];

                        shaderConstantBuffer.offset += static_cast<std::uint32_t>(getVectorSize(shaderData));

                        break;
                    }

                    case Command::Type::initTexture:
                    {
                        const auto initTextureCommand = static_cast<const InitTextureCommand*>(command.get());

                        auto texture = std::make_unique<Texture>(*this,
                                                                 initTextureCommand->levels,
                                                                 initTextureCommand->textureType,
                                                                 initTextureCommand->flags,
                                                                 initTextureCommand->sampleCount,
                                                                 initTextureCommand->pixelFormat,
                                                                 initTextureCommand->filter,
                                                                 initTextureCommand->maxAnisotropy);

                        if (initTextureCommand->texture > resources.size())
                            resources.resize(initTextureCommand->texture);
                        resources[initTextureCommand->texture - 1] = std::move(texture);
                        break;
                    }

                    case Command::Type::setTextureData:
                    {
                        const auto setTextureDataCommand = static_cast<const SetTextureDataCommand*>(command.get());

                        const auto texture = getResource<Texture>(setTextureDataCommand->texture);
                        texture->setData(setTextureDataCommand->levels);

                        break;
                    }

                    case Command::Type::setTextureParameters:
                    {
                        const auto setTextureParametersCommand = static_cast<const SetTextureParametersCommand*>(command.get());

                        const auto texture = getResource<Texture>(setTextureParametersCommand->texture);
                        texture->setFilter(setTextureParametersCommand->filter);
                        texture->setAddressX(setTextureParametersCommand->addressX);
                        texture->setAddressY(setTextureParametersCommand->addressY);
                        texture->setAddressZ(setTextureParametersCommand->addressZ);
                        texture->setMaxAnisotropy(setTextureParametersCommand->maxAnisotropy);

                        break;
                    }

                    case Command::Type::setTextures:
                    {
                        const auto setTexturesCommand = static_cast<const SetTexturesCommand*>(command.get());

                        if (!currentRenderCommandEncoder)
                            throw Error("Metal render command encoder not initialized");

                        for (std::uint32_t layer = 0; layer < setTexturesCommand->textures.size(); ++layer)
                        {
                            if (auto texture = getResource<Texture>(setTexturesCommand->textures[layer]))
                            {
                                [currentRenderCommandEncoder setFragmentTexture:texture->getTexture().get() atIndex:layer];
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

                    case Command::Type::renderScene: break;

                    default: throw Error("Invalid command");
                }

                if (command->type == Command::Type::present) return;
            }
        }
    }

    void RenderDevice::generateScreenshot(const std::string& filename)
    {
        if (!currentMetalTexture)
            throw Error("No back buffer");

        const auto width = static_cast<NSUInteger>(currentMetalTexture.get().width);
        const auto height = static_cast<NSUInteger>(currentMetalTexture.get().height);

        std::vector<std::uint8_t> data(width * height * 4);
        [currentMetalTexture.get() getBytes:data.data()
                                bytesPerRow:width * 4
                                 fromRegion:MTLRegionMake2D(0, 0, width, height)
                                mipmapLevel:0];

        for (std::uint32_t y = 0; y < height; ++y)
            for (std::uint32_t x = 0; x < width; ++x)
            {
                // reverse the order of channels
                const std::uint8_t temp = data[((y * width + x) * 4)];
                data[((y * width + x) * 4)] = data[((y * width + x) * 4) + 2];
                data[((y * width + x) * 4) + 2] = temp;
                data[((y * width + x) * 4) + 3] = 255;
            }

        if (!stbi_write_png(filename.c_str(),
                            static_cast<int>(width), static_cast<int>(height),
                            4, data.data(), static_cast<int>(width * 4)))
            throw Error("Failed to save image to file");
    }

    MTLRenderPipelineStatePtr RenderDevice::getPipelineState(const PipelineStateDesc& desc)
    {
        if (!desc.blendState || !desc.shader || !desc.sampleCount) return nil;

        const auto pipelineStateIterator = pipelineStates.find(desc);

        if (pipelineStateIterator != pipelineStates.end())
            return pipelineStateIterator->second.get();
        else
        {
            Pointer<MTLRenderPipelineDescriptor*> pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.get().sampleCount = desc.sampleCount;

            if (desc.shader)
            {
                assert(desc.shader->getFragmentShader());
                assert(desc.shader->getVertexShader());
                assert(desc.shader->getVertexDescriptor());

                pipelineStateDescriptor.get().vertexFunction = desc.shader->getVertexShader().get();
                pipelineStateDescriptor.get().fragmentFunction = desc.shader->getFragmentShader().get();
                pipelineStateDescriptor.get().vertexDescriptor = desc.shader->getVertexDescriptor().get();
            }

            for (std::size_t i = 0; i < desc.colorFormats.size(); ++i)
                pipelineStateDescriptor.get().colorAttachments[i].pixelFormat = desc.colorFormats[i];
            pipelineStateDescriptor.get().depthAttachmentPixelFormat = desc.depthFormat;
            pipelineStateDescriptor.get().stencilAttachmentPixelFormat = desc.stencilFormat;

            if (desc.blendState)
                for (std::size_t i = 0; i < desc.colorFormats.size(); ++i)
                {
                    // blending
                    pipelineStateDescriptor.get().colorAttachments[i].blendingEnabled = desc.blendState->isBlendingEnabled() ? YES : NO;

                    pipelineStateDescriptor.get().colorAttachments[i].sourceRGBBlendFactor = desc.blendState->getSourceRgbBlendFactor();
                    pipelineStateDescriptor.get().colorAttachments[i].destinationRGBBlendFactor = desc.blendState->getDestinationRgbBlendFactor();
                    pipelineStateDescriptor.get().colorAttachments[i].rgbBlendOperation = desc.blendState->getRgbBlendOperation();

                    pipelineStateDescriptor.get().colorAttachments[i].sourceAlphaBlendFactor = desc.blendState->getSourceAlphaBlendFactor();
                    pipelineStateDescriptor.get().colorAttachments[i].destinationAlphaBlendFactor = desc.blendState->getDestinationAlphaBlendFactor();
                    pipelineStateDescriptor.get().colorAttachments[i].alphaBlendOperation = desc.blendState->getAlphaBlendOperation();

                    pipelineStateDescriptor.get().colorAttachments[i].writeMask = desc.blendState->getColorWriteMask();
                }

            NSError* error;
            Pointer<id<MTLRenderPipelineState>> pipelineState = [device.get()
                                                                 newRenderPipelineStateWithDescriptor:pipelineStateDescriptor.get()
                                                                                                error:&error];
            if (error || !pipelineState)
                throw Error("Failed to created Metal pipeline state");

            auto result = pipelineState.get();

            pipelineStates[desc] = std::move(pipelineState);

            return result;
        }
    }

    MTLSamplerStatePtr RenderDevice::getSamplerState(const SamplerStateDescriptor& descriptor)
    {
        const auto samplerStatesIterator = samplerStates.find(descriptor);

        if (samplerStatesIterator != samplerStates.end())
            return samplerStatesIterator->second.get();
        else
        {
            Pointer<MTLSamplerDescriptor*> samplerDescriptor = [[MTLSamplerDescriptor alloc] init];
            switch (descriptor.filter)
            {
                case SamplerFilter::point:
                    samplerDescriptor.get().minFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.get().magFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.get().mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case SamplerFilter::linear:
                    samplerDescriptor.get().minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.get().magFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.get().mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case SamplerFilter::bilinear:
                    samplerDescriptor.get().minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.get().magFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.get().mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case SamplerFilter::trilinear:
                    samplerDescriptor.get().minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.get().magFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.get().mipFilter = MTLSamplerMipFilterLinear;
                    break;
                default:
                    throw Error("Invalid texture filter");
            }

            samplerDescriptor.get().sAddressMode = getSamplerAddressMode(descriptor.addressX);
            samplerDescriptor.get().tAddressMode = getSamplerAddressMode(descriptor.addressY);
            samplerDescriptor.get().rAddressMode = getSamplerAddressMode(descriptor.addressZ);

            samplerDescriptor.get().maxAnisotropy = descriptor.maxAnisotropy;

            MTLSamplerStatePtr samplerState = [device.get() newSamplerStateWithDescriptor:samplerDescriptor.get()];

            if (!samplerState)
                throw Error("Failed to create Metal sampler state");

            samplerStates[descriptor] = samplerState;

            return samplerState;
        }
    }
}

#endif
