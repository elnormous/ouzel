// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#ifdef __OBJC__
#import <Metal/Metal.h>
typedef id<MTLDevice> MTLDevicePtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
#else
#include <objc/objc.h>
typedef id MTLDevicePtr;
typedef id MTLCommandQueuePtr;
#endif

#include "Renderer.h"

namespace ouzel
{
    namespace video
    {
        class RendererMetal: public Renderer
        {
            friend Engine;
        public:
            virtual ~RendererMetal();

            virtual bool init(const Size2& size, bool fullscreen) override;

            virtual void setClearColor(Color color) override;

            virtual void setSize(const Size2& size) override;

            virtual void clear() override;
            virtual void flush() override;

            virtual TexturePtr loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps = true) override;
            virtual TexturePtr loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;

            virtual ShaderPtr loadShaderFromFiles(const std::string& fragmentShader,
                                                  const std::string& vertexShader,
                                                  uint32_t vertexAttributes,
                                                  const std::string& fragmentShaderFunction = "",
                                                  const std::string& vertexShaderFunction = "") override;
            virtual ShaderPtr loadShaderFromBuffers(const uint8_t* fragmentShader,
                                                    uint32_t fragmentShaderSize,
                                                    const uint8_t* vertexShader,
                                                    uint32_t vertexShaderSize,
                                                    uint32_t vertexAttributes,
                                                    const std::string& fragmentShaderFunction = "",
                                                    const std::string& vertexShaderFunction = "") override;

            virtual MeshBufferPtr createMeshBuffer() override;
            virtual MeshBufferPtr createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer) override;
            virtual bool drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST) override;

            MTLDevicePtr getDevice() const { return _device; }

        private:
            RendererMetal();

            void destroy();

            MTLDevicePtr _device = Nil;
            MTLCommandQueuePtr _commandQueue = Nil;
        };
    } // namespace video
} // namespace ouzel
