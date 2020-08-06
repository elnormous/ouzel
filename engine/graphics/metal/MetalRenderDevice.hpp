// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICE_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <map>
#include <memory>
#include <vector>

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

#include "../RenderDevice.hpp"
#include "MetalPointer.hpp"
#include "MetalShader.hpp"
#include "MetalTexture.hpp"
#include "DispatchSemaphore.hpp"

namespace ouzel::graphics::metal
{
    class BlendState;
    class Shader;

    class RenderDevice: public graphics::RenderDevice
    {
        friend Graphics;
    public:
        static constexpr std::size_t bufferSize = 1024U * 1024U; // size of shader constant buffer
        static constexpr std::size_t bufferCount = 3U; // allow encoding up to 3 command buffers simultaneously

        static bool available();

        auto& getDevice() const noexcept { return device; }

        MTLSamplerStatePtr getSamplerState(const SamplerStateDescriptor& descriptor);

        template <class T>
        auto getResource(std::size_t id) const
        {
            return id ? static_cast<T*>(resources[id - 1].get()) : nullptr;
        }

    protected:
        explicit RenderDevice(core::Window& newWindow,
                              const std::function<void(const Event&)>& initCallback);

        void init(const Settings& settings) override;

        void process() override;
        void generateScreenshot(const std::string& filename) override;

        class PipelineStateDesc final
        {
        public:
            BlendState* blendState = nullptr;
            Shader* shader = nullptr;
            NSUInteger sampleCount = 0;
            std::vector<MTLPixelFormat> colorFormats;
            MTLPixelFormat depthFormat;
            MTLPixelFormat stencilFormat;

            bool operator<(const PipelineStateDesc& other) const noexcept
            {
                return std::tie(blendState, shader, sampleCount, colorFormats, depthFormat) <
                    std::tie(other.blendState, other.shader, other.sampleCount, colorFormats, other.depthFormat);
            }
        };

        MTLRenderPipelineStatePtr getPipelineState(const PipelineStateDesc& desc);

        Pointer<MTLDevicePtr> device;
        Pointer<MTLCommandQueuePtr> metalCommandQueue;
        CAMetalLayerPtr metalLayer = nil;
        Pointer<MTLTexturePtr> currentMetalTexture;

        struct ShaderConstantBuffer final
        {
            std::vector<Pointer<MTLBufferPtr>> buffers;
            std::uint32_t index = 0;
            std::uint32_t offset = 0;
        };

        std::uint32_t shaderConstantBufferIndex = 0;
        ShaderConstantBuffer shaderConstantBuffers[bufferCount];

        Pointer<MTLRenderPassDescriptorPtr> renderPassDescriptor;
        Pointer<MTLDepthStencilStatePtr> defaultDepthStencilState;

        Pointer<MTLTexturePtr> msaaTexture;
        Pointer<MTLTexturePtr> depthTexture;
        std::map<SamplerStateDescriptor, Pointer<MTLSamplerStatePtr>> samplerStates;

        MTLPixelFormat colorFormat;
        MTLPixelFormat depthFormat;
        MTLPixelFormat stencilFormat;

        DispatchSemaphore inflightSemaphore{bufferCount};

        std::map<PipelineStateDesc, Pointer<MTLRenderPipelineStatePtr>> pipelineStates;

        std::vector<std::unique_ptr<RenderResource>> resources;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICE_HPP
