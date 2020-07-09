// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Buffer.hpp"
#include "Graphics.hpp"

namespace ouzel::graphics
{
    Buffer::Buffer(Renderer& initRenderer):
        renderer(&initRenderer),
        resource(initRenderer.getDevice()->createResource())
    {
    }

    Buffer::Buffer(Renderer& initRenderer,
                   BufferType initType,
                   Flags initFlags,
                   std::uint32_t initSize):
        renderer(&initRenderer),
        resource(initRenderer.getDevice()->createResource()),
        type(initType),
        flags(initFlags),
        size(initSize)
    {
        initRenderer.addCommand(std::make_unique<InitBufferCommand>(resource,
                                                                    initType,
                                                                    initFlags,
                                                                    std::vector<std::uint8_t>(),
                                                                    initSize));
    }

    Buffer::Buffer(Renderer& initRenderer,
                   BufferType initType,
                   Flags initFlags,
                   const void* initData,
                   std::uint32_t initSize):
        renderer(&initRenderer),
        resource(initRenderer.getDevice()->createResource()),
        type(initType),
        flags(initFlags),
        size(initSize)
    {
        initRenderer.addCommand(std::make_unique<InitBufferCommand>(resource,
                                                                    initType,
                                                                    initFlags,
                                                                    std::vector<std::uint8_t>(static_cast<const std::uint8_t*>(initData),
                                                                                         static_cast<const std::uint8_t*>(initData) + initSize),
                                                                    initSize));
    }

    Buffer::Buffer(Renderer& initRenderer,
                   BufferType initType,
                   Flags initFlags,
                   const std::vector<std::uint8_t>& initData,
                   std::uint32_t initSize):
        renderer(&initRenderer),
        resource(initRenderer.getDevice()->createResource()),
        type(initType),
        flags(initFlags),
        size(initSize)
    {
        if (!initData.empty() && initSize != initData.size())
            throw std::runtime_error("Invalid buffer data");

        initRenderer.addCommand(std::make_unique<InitBufferCommand>(resource,
                                                                    initType,
                                                                    initFlags,
                                                                    initData,
                                                                    initSize));
    }

    void Buffer::setData(const void* newData, std::uint32_t newSize)
    {
        if (resource)
            renderer->addCommand(std::make_unique<SetBufferDataCommand>(resource,
                                                                        std::vector<std::uint8_t>(static_cast<const std::uint8_t*>(newData),
                                                                                             static_cast<const std::uint8_t*>(newData) + newSize)));
    }

    void Buffer::setData(const std::vector<std::uint8_t>& newData)
    {
        if ((flags & Flags::dynamic) != Flags::dynamic)
            throw std::runtime_error("Buffer is not dynamic");

        if (newData.empty())
            throw std::runtime_error("Invalid buffer data");

        if (newData.size() > size) size = static_cast<std::uint32_t>(newData.size());

        if (resource)
            renderer->addCommand(std::make_unique<SetBufferDataCommand>(resource, newData));
    }
}
