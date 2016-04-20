// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <dispatch/dispatch.h>

#ifdef __OBJC__
#import <Metal/Metal.h>
#import <MetalKit/MTKView.h>
typedef MTKView* MTKViewPtr;
typedef id<MTLDevice> MTLDevicePtr;
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
typedef id<MTLRenderPipelineState> MTLRenderPipelineStatePtr;
typedef id<MTLCommandBuffer> MTLCommandBufferPtr;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderPtr;
#else
#include <objc/objc.h>
typedef id MTKViewPtr;
typedef id MTLDevicePtr;
typedef id MTLSamplerStatePtr;
typedef id MTLCommandQueuePtr;
typedef id MTLRenderPipelineStatePtr;
typedef id MTLCommandBufferPtr;
typedef id MTLRenderCommandEncoderPtr;
#endif

#include "Renderer.h"

namespace ouzel
{
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

            virtual bool init(const Size2& size, bool fullscreen) override;

            virtual void setClearColor(Color color) override;

            virtual void setSize(const Size2& size) override;

            virtual void clear() override;
            virtual void present() override;

            virtual TexturePtr loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps = true) override;
            virtual TexturePtr loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;

            virtual ShaderPtr loadShaderFromFiles(const std::string& pixelShader,
                                                  const std::string& vertexShader,
                                                  uint32_t vertexAttributes,
                                                  const std::string& pixelShaderFunction = "",
                                                  const std::string& vertexShaderFunction = "") override;
            virtual ShaderPtr loadShaderFromBuffers(const uint8_t* pixelShader,
                                                    uint32_t pixelShaderSize,
                                                    const uint8_t* vertexShader,
                                                    uint32_t vertexShaderSize,
                                                    uint32_t vertexAttributes,
                                                    const std::string& pixelShaderFunction = "",
                                                    const std::string& vertexShaderFunction = "") override;

            virtual MeshBufferPtr createMeshBuffer() override;
            virtual MeshBufferPtr createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer) override;
            virtual bool drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0) override;

            MTLDevicePtr getDevice() const { return _device; }

        private:
            RendererMetal();

            void destroy();
            MTLRenderPipelineStatePtr createPipelineState(const std::shared_ptr<BlendStateMetal>& blendState,
                                                          const std::shared_ptr<ShaderMetal>& shader);

            MTKViewPtr _view = Nil;

            MTLDevicePtr _device = Nil;

            MTLSamplerStatePtr _samplerState = Nil;
            MTLCommandQueuePtr _commandQueue = Nil;

            MTLCommandBufferPtr _currentCommandBuffer = Nil;
            MTLRenderCommandEncoderPtr _currentRenderCommandEncoder = Nil;

            dispatch_semaphore_t _inflightSemaphore;

            std::map<std::pair<std::shared_ptr<BlendStateMetal>, std::shared_ptr<ShaderMetal>>, MTLRenderPipelineStatePtr> _pipelineStates;
        };
    } // namespace graphics
} // namespace ouzel
