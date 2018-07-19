// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Buffer.hpp"
#include "BufferResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
    {
        Buffer::Buffer(Renderer& initRenderer):
            renderer(initRenderer)
        {
            resource = renderer.getDevice()->createBuffer();
        }

        Buffer::~Buffer()
        {
            if (resource) renderer.getDevice()->deleteResource(resource);
        }

        void Buffer::init(Usage newUsage, uint32_t newFlags, uint32_t newSize)
        {
            usage = newUsage;
            flags = newFlags;
            size = newSize;

            RenderDevice* renderDevice = renderer.getDevice();

            renderDevice->addCommand(InitBufferCommand(resource,
                                                       newUsage,
                                                       newFlags,
                                                       std::vector<uint8_t>(),
                                                       newSize));
        }

        void Buffer::init(Usage newUsage, uint32_t newFlags, const void* newData, uint32_t newSize)
        {
            init(newUsage,
                 newFlags,
                 std::vector<uint8_t>(static_cast<const uint8_t*>(newData),
                                      static_cast<const uint8_t*>(newData) + newSize),
                 newSize);
        }

        void Buffer::init(Usage newUsage, uint32_t newFlags, const std::vector<uint8_t>& newData, uint32_t newSize)
        {
            if (!newData.empty() && newSize != newData.size())
                throw DataError("Invalid buffer data");

            usage = newUsage;
            flags = newFlags;
            size = newSize;

            RenderDevice* renderDevice = renderer.getDevice();

            renderDevice->addCommand(InitBufferCommand(resource,
                                                       newUsage,
                                                       newFlags,
                                                       newData,
                                                       newSize));
        }

        void Buffer::setData(const void* newData, uint32_t newSize)
        {
            RenderDevice* renderDevice = renderer.getDevice();

            renderDevice->addCommand(SetBufferDataCommand(resource,
                                                          std::vector<uint8_t>(static_cast<const uint8_t*>(newData),
                                                                               static_cast<const uint8_t*>(newData) + newSize)));
        }

        void Buffer::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Buffer::DYNAMIC))
                throw DataError("Buffer is not dynamic");

            if (newData.empty())
                throw DataError("Invalid buffer data");

            if (newData.size() > size) size = static_cast<uint32_t>(newData.size());

            RenderDevice* renderDevice = renderer.getDevice();

            renderDevice->addCommand(SetBufferDataCommand(resource,
                                                          newData));
        }
    } // namespace graphics
} // namespace ouzel
