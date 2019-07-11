// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <algorithm>
#include <stdexcept>
#include "MetalBuffer.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            Buffer::Buffer(RenderDevice& initRenderDevice,
                           BufferType initType,
                           uint32_t initFlags,
                           const std::vector<uint8_t>& data,
                           uint32_t initSize):
                RenderResource(initRenderDevice),
                type(initType),
                flags(initFlags)
            {
                createBuffer(initSize);

                if (!data.empty())
                    std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
            }

            Buffer::~Buffer()
            {
                if (buffer) [buffer release];
            }

            void Buffer::setData(const std::vector<uint8_t>& data)
            {
                if (!(flags & Flags::Dynamic))
                    throw std::runtime_error("Buffer is not dynamic");

                if (data.empty())
                    throw std::runtime_error("Data is empty");

                if (!buffer || data.size() > size)
                    createBuffer(static_cast<uint32_t>(data.size()));

                std::copy(data.begin(), data.end(), static_cast<uint8_t*>([buffer contents]));
            }

            void Buffer::createBuffer(NSUInteger newSize)
            {
                if (buffer)
                {
                    [buffer release];
                    buffer = nil;
                }

                if (newSize > 0)
                {
                    size = newSize;

                    buffer = [renderDevice.getDevice() newBufferWithLength:size
                                                                   options:MTLResourceCPUCacheModeWriteCombined];

                    if (!buffer)
                        throw std::runtime_error("Failed to create Metal buffer");
                }
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
