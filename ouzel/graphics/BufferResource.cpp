// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "BufferResource.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferResource::BufferResource(RenderDevice& initRenderDevice):
            renderDevice(initRenderDevice)
        {
        }

        BufferResource::~BufferResource()
        {
        }

        void BufferResource::init(Buffer::Usage newUsage, uint32_t newFlags,
                                  const std::vector<uint8_t>& newData,
                                  uint32_t)
        {
            usage = newUsage;
            flags = newFlags;
            data = newData;
        }
    } // namespace graphics
} // namespace ouzel
