// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "BufferInterface.h"

namespace ouzel
{
    namespace graphics
    {
        BufferInterface::BufferInterface()
        {
        }

        BufferInterface::~BufferInterface()
        {
        }

        bool BufferInterface::init(Buffer::Usage newUsage, uint32_t newFlags)
        {
            usage = newUsage;
            flags = newFlags;

            return true;
        }

        bool BufferInterface::init(Buffer::Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags)
        {
            usage = newUsage;
            flags = newFlags;
            data = newData;

            return true;
        }

        bool BufferInterface::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Buffer::DYNAMIC))
            {
                return false;
            }

            data = newData;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
