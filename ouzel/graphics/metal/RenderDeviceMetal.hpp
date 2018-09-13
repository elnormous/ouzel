// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <map>
#include <vector>
#include <dispatch/dispatch.h>

#if defined(__OBJC__)
#import <CoreVideo/CoreVideo.h>
#import <QuartzCore/QuartzCore.h>
#import <Metal/Metal.h>
typedef CAMetalLayer* CAMetalLayerPtr;
typedef id<MTLDevice> MTLDevicePtr;
typedef id<MTLBuffer> MTLBufferPtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLCommandBuffer> MTLCommandBufferPtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderPtr;
typedef id<MTLRenderPipelineState> MTLRenderPipelineStatePtr;
typedef id<MTLTexture> MTLTexturePtr;
typedef id<MTLDepthStencilState> MTLDepthStencilStatePtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
typedef id CAMetalLayerPtr;
typedef id MTLDevicePtr;
typedef id MTLBufferPtr;
typedef id MTLRenderPassDescriptorPtr;
typedef id MTLSamplerStatePtr;
typedef id MTLCommandBufferPtr;
typedef id MTLCommandQueuePtr;
typedef id MTLRenderCommandEncoderPtr;
typedef id MTLRenderPipelineStatePtr;
typedef id MTLTexturePtr;
typedef id MTLDepthStencilStatePtr;
typedef NSUInteger MTLPixelFormat;
#define MTLPixelFormatInvalid 0
typedef NSUInteger MTLLoadAction;
#define MTLLoadActionDontCare 0
#define MTLLoadActionClear 2
#endif

#include "graphics/RenderDevice.hpp"
#include "graphics/metal/ShaderResourceMetal.hpp"
#include "graphics/metal/TextureResourceMetal.hpp"

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResourceMetal;
        class ShaderResourceMetal;

        class RenderDeviceMetal: public RenderDevice
        {
            friend Renderer;
        public:
            static const size_t BUFFER_SIZE = 1024 * 1024; // size of shader constant buffer
            static const size_t BUFFER_COUNT = 3; // allow encoding up to 3 command buffers simultaneously

            static bool available();

            virtual ~RenderDeviceMetal();

            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color newClearColor);
            void setClearDepth(float newClearDepth);

            inline MTLDevicePtr getDevice() const { return device; }

            MTLSamplerStatePtr getSamplerState(const SamplerStateDescriptor& descriptor);

        protected:
            RenderDeviceMetal();

            virtual void init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual void setSize(const Size2& newSize) override;

            virtual void process() override;
            virtual void generateScreenshot(const std::string& filename) override;

            virtual RenderResource* createBlendState() override;
            virtual RenderResource* createBuffer() override;
            virtual RenderResource* createRenderTarget() override;
            virtual RenderResource* createShader() override;
            virtual RenderResource* createTexture() override;

            class PipelineStateDesc
            {
            public:
                BlendStateResourceMetal* blendState = nullptr;
                ShaderResourceMetal* shader = nullptr;
                NSUInteger sampleCount = 0;
                MTLPixelFormat colorFormat;
                MTLPixelFormat depthFormat;

                bool operator<(const PipelineStateDesc& other) const
                {
                    return std::tie(blendState, shader, sampleCount, colorFormat, depthFormat) <
                        std::tie(other.blendState, other.shader, other.sampleCount, colorFormat, other.depthFormat);
                }
            };

            MTLRenderPipelineStatePtr getPipelineState(const PipelineStateDesc& desc);

            MTLDevicePtr device = nil;
            MTLCommandQueuePtr metalCommandQueue = nil;
            CAMetalLayerPtr metalLayer = nil;
            MTLTexturePtr currentMetalTexture = nullptr;

            struct ShaderConstantBuffer
            {
                MTLBufferPtr buffer = nil;
                uint32_t offset = 0;
            };

            uint32_t shaderConstantBufferIndex = 0;
            ShaderConstantBuffer shaderConstantBuffers[BUFFER_COUNT];

            MTLRenderPassDescriptorPtr renderPassDescriptor = nil;

            MTLDepthStencilStatePtr depthStencilStates[4];

            MTLTexturePtr msaaTexture = nil;
            MTLTexturePtr depthTexture = nil;
            std::map<SamplerStateDescriptor, MTLSamplerStatePtr> samplerStates;

            MTLPixelFormat colorFormat = MTLPixelFormatInvalid;
            MTLPixelFormat depthFormat = MTLPixelFormatInvalid;

            MTLLoadAction colorBufferLoadAction = MTLLoadActionClear;
            MTLLoadAction depthBufferLoadAction = MTLLoadActionDontCare;

            dispatch_semaphore_t inflightSemaphore;

            std::map<PipelineStateDesc, MTLRenderPipelineStatePtr> pipelineStates;
        };
    } // namespace graphics
} // namespace ouzel

#endif
