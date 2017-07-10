// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

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
typedef void* CVDisplayLinkRef;
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
typedef NSUInteger MTLColorWriteMask;
#endif

#include "graphics/Renderer.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class BlendStateResourceMetal;
        class ShaderResourceMetal;

        class RendererMetal: public Renderer
        {
            friend Engine;
        public:
            static bool available();

            virtual ~RendererMetal();

            virtual std::vector<Size2> getSupportedResolutions() const override;

            MTLDevicePtr getDevice() const { return device; }

            struct SamplerStateDesc
            {
                Texture::Filter filter;
                Texture::Address addressX;
                Texture::Address addressY;
                uint32_t maxAnisotropy;

                bool operator<(const SamplerStateDesc& other) const
                {
                    return std::tie(filter, addressX, addressY, maxAnisotropy) < std::tie(other.filter, other.addressX, other.addressY, other.maxAnisotropy);
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
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;
            virtual bool upload() override;
            virtual bool draw(const std::vector<DrawCommand>& drawCommands) override;
            virtual bool generateScreenshot(const std::string& filename) override;

            virtual BlendStateResource* createBlendState() override;
            virtual TextureResource* createTexture() override;
            virtual ShaderResource* createShader() override;
            virtual MeshBufferResource* createMeshBuffer() override;
            virtual BufferResource* createBuffer() override;

            struct PipelineStateDesc
            {
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

            MTLDevicePtr device = Nil;
            MTLCommandQueuePtr commandQueue = Nil;
            CAMetalLayerPtr metalLayer = Nil;
            MTLTexturePtr currentMetalTexture = nullptr;

            struct ShaderConstantBuffer
            {
                MTLBufferPtr buffer = Nil;
                uint32_t offset = 0;
            };

            uint32_t shaderConstantBufferIndex = 0;
            std::vector<ShaderConstantBuffer> shaderConstantBuffers;

            MTLRenderPassDescriptorPtr renderPassDescriptor = Nil;

            MTLDepthStencilStatePtr depthStencilStates[4];

            MTLTexturePtr msaaTexture = Nil;
            MTLTexturePtr depthTexture = Nil;
            std::map<SamplerStateDesc, MTLSamplerStatePtr> samplerStates;

            MTLPixelFormat colorFormat = MTLPixelFormatInvalid;
            MTLPixelFormat depthFormat = MTLPixelFormatInvalid;

            MTLLoadAction colorBufferLoadAction = MTLLoadActionDontCare;
            MTLLoadAction depthBufferLoadAction = MTLLoadActionDontCare;

            dispatch_semaphore_t inflightSemaphore;

            std::map<PipelineStateDesc, MTLRenderPipelineStatePtr> pipelineStates;
        };
    } // namespace graphics
} // namespace ouzel

#endif
