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
            if (!resource->init(newUsage, newDynamic))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Buffer::initFromBuffer(Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic)
        {
            if (!resource->initFromBuffer(newUsage, newData, newSize, newDynamic))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Buffer::setData(const void* newData, uint32_t newSize)
        {
            if (!resource->setData(newData, newSize))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

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
