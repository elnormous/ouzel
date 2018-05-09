// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
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
typedef id MTLCommandQueuePtr;
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

namespace ouzel
{
    namespace graphics
    {
        class BlendStateResourceMetal;
        class ShaderResourceMetal;

        class RenderDeviceMetal: public RenderDevice
        {
            friend RenderDevice;
        public:
            static bool available();

            virtual ~RenderDeviceMetal();

            virtual void setClearColorBuffer(bool clear) override;
            virtual void setClearDepthBuffer(bool clear) override;
            virtual void setClearColor(Color color) override;
            virtual void setClearDepth(float newClearDepth) override;

            inline MTLDevicePtr getDevice() const { return device; }

            class SamplerStateDescriptor
            {
            public:
                Texture::Filter filter;
                Texture::Address addressX;
                Texture::Address addressY;
                uint32_t maxAnisotropy;

                bool operator<(const SamplerStateDescriptor& other) const
                {
                    return std::tie(filter, addressX, addressY, maxAnisotropy) < std::tie(other.filter, other.addressX, other.addressY, other.maxAnisotropy);
                }
            };

            MTLSamplerStatePtr getSamplerState(const SamplerStateDescriptor& descriptor);

        protected:
            RenderDeviceMetal();

            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual void setSize(const Size2& newSize) override;

            virtual bool processCommands(const std::vector<std::unique_ptr<Command>>& commands) override;
            virtual bool generateScreenshot(const std::string& filename) override;

            virtual BlendStateResource* createBlendState() override;
            virtual TextureResource* createTexture() override;
            virtual ShaderResource* createShader() override;
            virtual MeshBufferResource* createMeshBuffer() override;
            virtual BufferResource* createBuffer() override;

            class PipelineStateDesc
            {
            public:
                BlendStateResourceMetal* blendState;
                ShaderResourceMetal* shader;
                NSUInteger sampleCount;
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
            MTLCommandQueuePtr commandQueue = nil;
            CAMetalLayerPtr metalLayer = nil;
            MTLTexturePtr currentMetalTexture = nullptr;

            struct ShaderConstantBuffer
            {
                MTLBufferPtr buffer = nil;
                uint32_t offset = 0;
            };

            uint32_t shaderConstantBufferIndex = 0;
            std::vector<ShaderConstantBuffer> shaderConstantBuffers;

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
