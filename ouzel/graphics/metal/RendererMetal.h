// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <dispatch/dispatch.h>

#if defined(__OBJC__)
#import <Metal/Metal.h>
#import <MetalKit/MTKView.h>
typedef MTKView* MTKViewPtr;
typedef id<MTLDevice> MTLDevicePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
typedef id<MTLRenderPipelineState> MTLRenderPipelineStatePtr;
typedef id<MTLCommandBuffer> MTLCommandBufferPtr;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderPtr;
typedef id<MTLTexture> MTLTextureResourcePtr;
typedef id<MTLDepthStencilState> MTLDepthStencilStatePtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
typedef id MTKViewPtr;
typedef id MTLDevicePtr;
typedef id MTLRenderPassDescriptorPtr;
typedef id MTLSamplerStatePtr;
typedef id MTLCommandQueuePtr;
typedef id MTLRenderPipelineStatePtr;
typedef id MTLCommandBufferPtr;
typedef id MTLRenderCommandEncoderPtr;
typedef id MTLTextureResourcePtr;
typedef id MTLDepthStencilStatePtr;
#endif

#include "graphics/Renderer.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class BlendStateMetal;
        class ShaderMetal;

        class RendererMetal: public Renderer
        {
            friend Engine;
        public:
            static bool available();

            virtual ~RendererMetal();

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual BlendStateResource* createBlendState() override;
            virtual TextureResource* createTexture() override;
            virtual ShaderResource* createShader() override;
            virtual MeshBufferResource* createMeshBuffer() override;
            virtual BufferResource* createBuffer() override;

            MTLDevicePtr getDevice() const { return device; }
            MTKViewPtr getMetalView() const { return view; }

            struct SamplerStateDesc
            {
                Texture::Filter filter;
                Texture::Address addressX;
                Texture::Address addressY;
                uint32_t maxAnisotropy;

                bool operator<(const SamplerStateDesc& other) const
                {
                    return std::tie(filter, addressX, addressY, maxAnisotropy) < std::tie(filter, addressX, addressY, maxAnisotropy);
                }
            };

            MTLSamplerStatePtr getSamplerState(const SamplerStateDesc& desc);

        protected:
            RendererMetal();

            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync,
                              bool newDepth) override;
            virtual bool upload() override;
            virtual bool draw(const std::vector<DrawCommand>& drawCommands) override;
            virtual bool generateScreenshot(const std::string& filename) override;

            struct PipelineStateDesc
            {
                BlendStateMetal* blendState;
                ShaderMetal* shader;
                NSUInteger sampleCount;
                NSUInteger colorFormat;
                NSUInteger depthFormat;

                bool operator<(const PipelineStateDesc& other) const
                {
                    return std::tie(blendState, shader, sampleCount, colorFormat, depthFormat) < std::tie(other.blendState, other.shader, other.sampleCount, colorFormat, other.depthFormat);
                }
            };

            MTLRenderPipelineStatePtr getPipelineState(const PipelineStateDesc& desc);

            bool createRenderCommandEncoder(MTLRenderPassDescriptorPtr newRenderPassDescriptor);

            MTKViewPtr view = Nil;

            MTLDevicePtr device = Nil;
            MTLRenderPassDescriptorPtr renderPassDescriptor = Nil;

            MTLDepthStencilStatePtr depthStencilStates[4];

            MTLTextureResourcePtr msaaTexture = Nil;
            MTLTextureResourcePtr depthTexture = Nil;
            std::map<SamplerStateDesc, MTLSamplerStatePtr> samplerStates;

            MTLCommandQueuePtr commandQueue = Nil;
            MTLCommandBufferPtr currentCommandBuffer = Nil;

            MTLRenderPassDescriptorPtr currentRenderPassDescriptor = Nil;
            MTLRenderCommandEncoderPtr currentRenderCommandEncoder = Nil;

            NSUInteger colorFormat = 0;
            NSUInteger depthFormat = 0;

            NSUInteger colorBufferLoadAction = 0;
            NSUInteger depthBufferLoadAction = 0;

            dispatch_semaphore_t inflightSemaphore;

            std::map<PipelineStateDesc, MTLRenderPipelineStatePtr> pipelineStates;
        };
    } // namespace graphics
} // namespace ouzel
