// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include "core/CompileConfig.h"
#include "graphics/MeshBuffer.h"

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
typedef id MTLBufferPtr;
typedef NSUInteger MTLIndexType;
#endif

namespace ouzel
{
    namespace graphics
    {
        class RendererMetal;

        class MeshBufferMetal: public MeshBuffer
        {
            friend RendererMetal;
        public:
            virtual ~MeshBufferMetal();
            virtual void free() override;

            virtual bool init(bool newDynamicIndexBuffer = true, bool newDynamicVertexBuffer = true) override;
            virtual bool initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                        uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                        const void* newVertices, uint32_t newVertexAttributes,
                                        uint32_t newVertexCount, bool newDynamicVertexBuffer) override;

            virtual bool setIndexSize(uint32_t indexSize) override;
            virtual bool setVertexAttributes(uint32_t vertexAttributes) override;

            virtual bool uploadIndices(const void* newIndices, uint32_t newIndexCount) override;
            virtual bool uploadVertices(const void* newVertices, uint32_t newVertexCount) override;

            MTLBufferPtr getIndexBuffer() const { return indexBuffer; }
            MTLBufferPtr getVertexBuffer() const { return vertexBuffer; }

            MTLIndexType getIndexFormat() const { return indexFormat; }
            NSUInteger getBytesPerIndex() const { return bytesPerIndex; }

        protected:
            MeshBufferMetal();
            virtual bool update() override;

            bool uploadData(MTLBufferPtr buffer, const std::vector<uint8_t>& data);

            MTLBufferPtr indexBuffer = Nil;
            uint32_t indexBufferSize = 0;

            MTLBufferPtr vertexBuffer = Nil;
            uint32_t vertexBufferSize = 0;

            MTLIndexType indexFormat;
            NSUInteger bytesPerIndex = 0;

            std::vector<uint8_t> indexData;
            std::vector<uint8_t> vertexData;
            std::atomic<bool> indexBufferDirty;
            std::atomic<bool> vertexBufferDirty;
            
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
