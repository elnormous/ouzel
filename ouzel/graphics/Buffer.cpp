// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Buffer.h"
#include "BufferResource.h"
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

        bool Buffer::init(Usage newUsage, bool newDynamic)
        {
            BufferResource* bufferResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([bufferResource,
                                                                newUsage,
                                                                newDynamic]() {
                bufferResource->init(newUsage, newDynamic);
            });

            return true;
        }

        bool Buffer::init(Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic)
        {
            return init(newUsage,
                        std::vector<uint8_t>(static_cast<const uint8_t*>(newData),
                                             static_cast<const uint8_t*>(newData) + newSize),
                        newDynamic);
        }

        bool Buffer::init(Usage newUsage, const std::vector<uint8_t>& newData, bool newDynamic)
        {
            BufferResource* bufferResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([bufferResource,
                                                                newUsage,
                                                                newData,
                                                                newDynamic]() {
                bufferResource->init(newUsage, newData, newDynamic);
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
            BufferResource* bufferResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([bufferResource,
                                                                newData]() {
                bufferResource->setData(newData);
            });

            return true;
        }

        bool Buffer::isDynamic() const
        {
            return resource->isDynamic();
        }

        Buffer::Usage Buffer::getUsage() const
        {
            return resource->getUsage();
        }

        uint32_t Buffer::getSize() const
        {
            return resource->getSize();
        }
    } // namespace graphics
} // namespace ouzel
