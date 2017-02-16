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
            if (resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool Buffer::init(Buffer::Usage newUsage, bool newDynamic)
        {
            usage = newUsage;
            dynamic = newDynamic;
            size = 0;

            if (!resource->init(newUsage, newDynamic))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Buffer::initFromBuffer(Buffer::Usage newUsage, const void* newData, uint32_t newSize, bool newDynamic)
        {
            usage = newUsage;
            dynamic = newDynamic;
            size = newSize;

            if (!resource->initFromBuffer(newUsage, newData, newSize, newDynamic))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Buffer::setData(const void* newData, uint32_t newSize)
        {
            if (!dynamic)
            {
                return false;
            }

            size = newSize;

            if (!resource->setData(newData, newSize))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
