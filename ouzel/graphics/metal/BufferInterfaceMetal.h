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
#endif

#include "graphics/BufferInterface.h"

namespace ouzel
{
    namespace graphics
    {
        class MeshBufferInterfaceMetal;

        class BufferInterfaceMetal: public BufferInterface
        {
            friend MeshBufferInterfaceMetal;
        public:
            BufferInterfaceMetal();
            virtual ~BufferInterfaceMetal();

            virtual bool init(Buffer::Usage newUsage, uint32_t newFlags = 0) override;
            virtual bool init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags = 0) override;

            virtual bool setData(const std::vector<uint8_t>& newData) override;

            MTLBufferPtr getBuffer() const { return buffer; }

        protected:
            MTLBufferPtr buffer = Nil;
            uint32_t bufferSize = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
