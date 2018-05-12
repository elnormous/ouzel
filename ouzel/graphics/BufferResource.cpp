// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BufferResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferResource::BufferResource()
        {
        }

        BufferResource::~BufferResource()
        {
        }

        bool BufferResource::init(Buffer::Usage newUsage, uint32_t newFlags,
                                  const std::vector<uint8_t>& newData,
                                  uint32_t)
        {
            usage = newUsage;
            flags = newFlags;
            data = newData;

            return true;
        }

        bool BufferResource::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Buffer::DYNAMIC))
                return false;

            if (newData.empty()) return false;

            data = newData;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
