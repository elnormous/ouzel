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

            BufferInterface* bufferResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([bufferResource,
                                                                newUsage,
                                                                newFlags,
                                                                newSize]() {
                bufferResource->init(newUsage, newFlags, newSize);
            });

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

            BufferInterface* bufferResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([bufferResource,
                                                                newUsage,
                                                                newData,
                                                                newFlags]() {
                bufferResource->init(newUsage, newData, newFlags);
            });

            return true;
        }

        bool Buffer::setData(const void* newData, uint32_t newSize)
        {
            return setData(std::vector<uint8_t>(static_cast<const uint8_t*>(newData),
                                                static_cast<const uint8_t*>(newData) + newSize));
        }

        bool Buffer::setData(const std::vector<uint8_t>& newData)
        {
            BufferInterface* bufferResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([bufferResource,
                                                                newData]() {
                bufferResource->setData(newData);
            });

            return true;
        }
    } // namespace graphics
} // namespace ouzel
