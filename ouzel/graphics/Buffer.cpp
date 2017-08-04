// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Buffer.h"
#include "BufferInterface.h"
#include "Renderer.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace graphics
    {
        Buffer::Buffer()
        {
            resource = sharedEngine->getRenderer()->createBuffer();
        }

        Buffer::~Buffer()
        {
            if (sharedEngine && resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool Buffer::init(Usage newUsage, uint32_t newFlags, uint32_t newSize)
        {
            usage = newUsage;
            flags = newFlags;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(BufferInterface::*)(Usage, uint32_t, uint32_t)>(&BufferInterface::init),
                                                                         resource,
                                                                         newUsage,
                                                                         newFlags,
                                                                         newSize));

            return true;
        }

        bool Buffer::init(Usage newUsage, const void* newData, uint32_t newSize, uint32_t newFlags)
        {
            return init(newUsage,
                        std::vector<uint8_t>(static_cast<const uint8_t*>(newData),
                                             static_cast<const uint8_t*>(newData) + newSize),
                        newFlags);
        }

        bool Buffer::init(Usage newUsage, const std::vector<uint8_t>& newData, uint32_t newFlags)
        {
            usage = newUsage;
            flags = newFlags;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(BufferInterface::*)(Buffer::Usage, const std::vector<uint8_t>&, uint32_t)>(&BufferInterface::init),
                                                                         resource,
                                                                         newUsage,
                                                                         newData,
                                                                         newFlags));

            return true;
        }

        bool Buffer::setData(const void* newData, uint32_t newSize)
        {
            return setData(std::vector<uint8_t>(static_cast<const uint8_t*>(newData),
                                                static_cast<const uint8_t*>(newData) + newSize));
        }

        bool Buffer::setData(const std::vector<uint8_t>& newData)
        {
            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&BufferInterface::setData,
                                                                         resource,
                                                                         newData));

            return true;
        }
    } // namespace graphics
} // namespace ouzel
