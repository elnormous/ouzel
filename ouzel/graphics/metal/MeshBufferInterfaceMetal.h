// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#if defined(__OBJC__)
#import <Metal/Metal.h>
typedef id<MTLBuffer> MTLBufferPtr;
#else
#include <objc/objc.h>
typedef id MTLBufferPtr;
typedef NSUInteger MTLIndexType;
#endif

#include "graphics/MeshBufferInterface.h"

namespace ouzel
{
    namespace graphics
    {
        class BufferInterfaceMetal;

        class MeshBufferInterfaceMetal: public MeshBufferInterface
        {
        public:
            MeshBufferInterfaceMetal();
            virtual ~MeshBufferInterfaceMetal();

            virtual bool init(uint32_t newIndexSize, BufferInterface* newIndexBuffer,
                              const std::vector<VertexAttribute>& newVertexAttributes, BufferInterface* newVertexBuffer) override;

            virtual bool setIndexSize(uint32_t newIndexSize) override;
            virtual bool setIndexBuffer(BufferInterface* newIndexBuffer) override;
            virtual bool setVertexAttributes(const std::vector<VertexAttribute>& newVertexAttributes) override;
            virtual bool setVertexBuffer(BufferInterface* newVertexBuffer) override;

            MTLIndexType getIndexType() const { return indexType; }
            NSUInteger getBytesPerIndex() const { return bytesPerIndex; }

            BufferInterfaceMetal* getIndexBufferMetal() const { return indexBufferMetal; }
            BufferInterfaceMetal* getVertexBufferMetal() const { return vertexBufferMetal; }

        protected:
            MTLIndexType indexType;
            NSUInteger bytesPerIndex = 0;

            BufferInterfaceMetal* indexBufferMetal = nullptr;
            BufferInterfaceMetal* vertexBufferMetal = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif
