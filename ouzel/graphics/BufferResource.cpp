// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BufferResource.hpp"
#include "utils/Errors.hpp"

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

        void BufferResource::init(Buffer::Usage newUsage, uint32_t newFlags,
                                  const std::vector<uint8_t>& newData,
                                  uint32_t)
        {
            usage = newUsage;
            flags = newFlags;
            data = newData;
        }

        void BufferResource::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Buffer::DYNAMIC))
                throw DataError("Buffer is not dynamic");

            if (newData.empty())
                throw DataError("Data is empty");

            data = newData;
        }
    } // namespace graphics
} // namespace ouzel
