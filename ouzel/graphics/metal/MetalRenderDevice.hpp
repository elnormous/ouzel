// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICE_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICE_HPP

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
typedef NSUInteger MTLLoadAction;
#endif

#include "graphics/RenderDevice.hpp"
#include "graphics/metal/MetalShader.hpp"
#include "graphics/metal/MetalTexture.hpp"

namespace ouzel
{
    namespace graphics
    {
        class MetalBlendState;
        class MetalShader;

        class MetalRenderDevice: public RenderDevice
        {
            friend Renderer;
        public:
            static constexpr size_t BUFFER_SIZE = 1024 * 1024; // size of shader constant buffer
            static constexpr size_t BUFFER_COUNT = 3; // allow encoding up to 3 command buffers simultaneously

            static bool available();

            ~MetalRenderDevice();

            inline MTLDevicePtr getDevice() const { return device; }

            MTLSamplerStatePtr getSamplerState(const SamplerStateDescriptor& descriptor);

        protected:
            MetalRenderDevice(const std::function<void(const Event&)>& initCallback);

            void init(Window* newWindow,
                      const Size2<uint32_t>& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newStencil,
                      bool newDebugRenderer) override;

            void process() override;
            void generateScreenshot(const std::string& filename) override;

            class PipelineStateDesc
            {
            public:
                MetalBlendState* blendState = nullptr;
                MetalShader* shader = nullptr;
                NSUInteger sampleCount = 0;
                std::vector<MTLPixelFormat> colorFormats;
                MTLPixelFormat depthFormat;
                MTLPixelFormat stencilFormat;

                bool operator<(const PipelineStateDesc& other) const
                {
                    return std::tie(blendState, shader, sampleCount, colorFormats, depthFormat) <
                        std::tie(other.blendState, other.shader, other.sampleCount, colorFormats, other.depthFormat);
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
            MTLDepthStencilStatePtr defaultDepthStencilState = nil;

            MTLTexturePtr msaaTexture = nil;
            MTLTexturePtr depthTexture = nil;
            std::map<SamplerStateDescriptor, MTLSamplerStatePtr> samplerStates;

            MTLPixelFormat colorFormat;
            MTLPixelFormat depthFormat;
            MTLPixelFormat stencilFormat;

            dispatch_semaphore_t inflightSemaphore;

            std::map<PipelineStateDesc, MTLRenderPipelineStatePtr> pipelineStates;

            std::vector<std::unique_ptr<MetalRenderResource>> resources;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICE_HPP
