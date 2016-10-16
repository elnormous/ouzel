// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
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
typedef id<MTLTexture> MTLTexturePtr;
#else
#include <objc/objc.h>
typedef id MTKViewPtr;
typedef id MTLDevicePtr;
typedef id MTLRenderPassDescriptorPtr;
typedef id MTLSamplerStatePtr;
typedef id MTLCommandQueuePtr;
typedef id MTLRenderPipelineStatePtr;
typedef id MTLCommandBufferPtr;
typedef id MTLRenderCommandEncoderPtr;
typedef id MTLTexturePtr;
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
            virtual void free() override;

            virtual void setClearColor(Color newColor) override;
            virtual void setSize(const Size2& newSize) override;

            virtual bool present() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual BlendStatePtr createBlendState() override;
            virtual TexturePtr createTexture() override;
            virtual RenderTargetPtr createRenderTarget() override;
            virtual ShaderPtr createShader() override;
            virtual MeshBufferPtr createMeshBuffer() override;
            virtual IndexBufferPtr createIndexBuffer() override;
            virtual VertexBufferPtr createVertexBuffer() override;

            MTLDevicePtr getDevice() const { return device; }
            MTKViewPtr getMetalView() const { return view; }

        protected:
            RendererMetal();

            virtual bool init(Window* newWindow,
                              uint32_t newSampleCount,
                              TextureFilter newTextureFilter,
                              PixelFormat newBackBufferFormat,
                              bool newVerticalSync) override;
            bool update();
            bool saveScreenshots();

            MTLRenderPipelineStatePtr createPipelineState(const std::shared_ptr<BlendStateMetal>& blendState,
                                                          const std::shared_ptr<ShaderMetal>& shader);

            bool createRenderCommandEncoder(MTLRenderPassDescriptorPtr newRenderPassDescriptor);

            MTKViewPtr view = Nil;

            MTLDevicePtr device = Nil;
            MTLRenderPassDescriptorPtr renderPassDescriptor = Nil;

            MTLSamplerStatePtr samplerState = Nil;
            MTLCommandQueuePtr commandQueue = Nil;

            MTLTexturePtr msaaTexture = Nil;

            MTLCommandBufferPtr currentCommandBuffer = Nil;

            MTLRenderPassDescriptorPtr currentRenderPassDescriptor = Nil;
            MTLRenderCommandEncoderPtr currentRenderCommandEncoder = Nil;

            dispatch_semaphore_t inflightSemaphore;

            std::map<std::pair<std::shared_ptr<BlendStateMetal>, std::shared_ptr<ShaderMetal>>, MTLRenderPipelineStatePtr> pipelineStates;

            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
