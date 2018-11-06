// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERDEVICEMETAL_HPP
#define RENDERDEVICEMETAL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <map>
#include <memory>
#include <vector>
#include <dispatch/dispatch.h>

#if defined(__OBJC__)
#  import <CoreVideo/CoreVideo.h>
#  import <QuartzCore/QuartzCore.h>
#  import <Metal/Metal.h>
typedef CAMetalLayer* CAMetalLayerPtr;
typedef id<MTLDevice> MTLDevicePtr;
typedef id<MTLBuffer> MTLBufferPtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
typedef id<MTLRenderPipelineState> MTLRenderPipelineStatePtr;
typedef id<MTLTexture> MTLTexturePtr;
typedef id<MTLDepthStencilState> MTLDepthStencilStatePtr;
#else
#  include <objc/objc.h>
#  include <objc/NSObjCRuntime.h>
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
#  define MTLPixelFormatInvalid 0
typedef NSUInteger MTLLoadAction;
#  define MTLLoadActionDontCare 0
#  define MTLLoadActionClear 2
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

            ~RenderDeviceMetal();

            inline MTLDevicePtr getDevice() const { return device; }

            MTLSamplerStatePtr getSamplerState(const SamplerStateDescriptor& descriptor);

        protected:
            RenderDeviceMetal();

            void init(Window* newWindow,
                      const Size2& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            void setSize(const Size2& newSize) override;

            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color newClearColor);
            void setClearDepth(float newClearDepth);

            void process() override;
            void generateScreenshot(const std::string& filename) override;

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
                std::vector<MTLBufferPtr> buffers;
                uint32_t index = 0;
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

            std::vector<std::unique_ptr<RenderResourceMetal>> resources;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // RENDERDEVICEMETAL_HPP
