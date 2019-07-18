// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Buffer.hpp"
#include "Renderer.hpp"

namespace ouzel
{
    namespace graphics
    {
        Buffer::Buffer(Renderer& initRenderer):
            renderer(&initRenderer),
            resource(initRenderer.getDevice()->createResource())
        {
        }

        Buffer::Buffer(Renderer& initRenderer,
                       BufferType initType,
                       uint32_t initFlags,
                       uint32_t initSize):
            renderer(&initRenderer),
            resource(initRenderer.getDevice()->createResource()),
            type(initType),
            flags(initFlags),
            size(initSize)
        {
            initRenderer.addCommand(std::make_unique<InitBufferCommand>(resource,
                                                                        initType,
                                                                        initFlags,
                                                                        std::vector<uint8_t>(),
                                                                        initSize));
        }

        Buffer::Buffer(Renderer& initRenderer,
                       BufferType initType,
                       uint32_t initFlags,
                       const void* initData,
                       uint32_t initSize):
            renderer(&initRenderer),
            resource(initRenderer.getDevice()->createResource()),
            type(initType),
            flags(initFlags),
            size(initSize)
        {
            initRenderer.addCommand(std::make_unique<InitBufferCommand>(resource,
                                                                        initType,
                                                                        initFlags,
                                                                        std::vector<uint8_t>(static_cast<const uint8_t*>(initData),
                                                                                             static_cast<const uint8_t*>(initData) + initSize),
                                                                        initSize));
        }

        Buffer::Buffer(Renderer& initRenderer,
                       BufferType initType,
                       uint32_t initFlags,
                       const std::vector<uint8_t>& initData,
                       uint32_t initSize):
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

        void Buffer::setData(const void* newData, uint32_t newSize)
        {
            if (resource)
                renderer->addCommand(std::make_unique<SetBufferDataCommand>(resource,
                                                                            std::vector<uint8_t>(static_cast<const uint8_t*>(newData),
                                                                                                 static_cast<const uint8_t*>(newData) + newSize)));
        }

        void Buffer::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Flags::Dynamic))
                throw std::runtime_error("Buffer is not dynamic");

            if (newData.empty())
                throw std::runtime_error("Invalid buffer data");

            if (newData.size() > size) size = static_cast<uint32_t>(newData.size());

            if (resource)
                renderer->addCommand(std::make_unique<SetBufferDataCommand>(resource, newData));
        }
    } // namespace graphics
} // namespace ouzel
