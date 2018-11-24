// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Buffer.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        Buffer::Buffer(Renderer& initRenderer):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId())
        {
        }

        Buffer::Buffer(Renderer& initRenderer, Usage initUsage, uint32_t initFlags, uint32_t initSize):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId()),
            usage(initUsage),
            flags(initFlags),
            size(initSize)
        {
            renderer.addCommand(std::unique_ptr<Command>(new InitBufferCommand(resource,
                                                                               initUsage,
                                                                               initFlags,
                                                                               std::vector<uint8_t>(),
                                                                               initSize)));
        }

        Buffer::Buffer(Renderer& initRenderer, Usage initUsage, uint32_t initFlags, const void* initData, uint32_t initSize):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId()),
            usage(initUsage),
            flags(initFlags),
            size(initSize)
        {
            renderer.addCommand(std::unique_ptr<Command>(new InitBufferCommand(resource,
                                                                               initUsage,
                                                                               initFlags,
                                                                               std::vector<uint8_t>(static_cast<const uint8_t*>(initData),
                                                                                                    static_cast<const uint8_t*>(initData) + initSize),
                                                                               initSize)));
        }

        Buffer::Buffer(Renderer& initRenderer, Usage initUsage, uint32_t initFlags, const std::vector<uint8_t>& initData, uint32_t initSize):
            renderer(initRenderer),
            resource(renderer.getDevice()->getResourceId()),
            usage(initUsage),
            flags(initFlags),
            size(initSize)
        {
            if (!initData.empty() && initSize != initData.size())
                throw std::runtime_error("Invalid buffer data");

            renderer.addCommand(std::unique_ptr<Command>(new InitBufferCommand(resource,
                                                                               initUsage,
                                                                               initFlags,
                                                                               initData,
                                                                               initSize)));
        }

        Buffer::~Buffer()
        {
            if (resource)
            {
                renderer.addCommand(std::unique_ptr<Command>(new DeleteResourceCommand(resource)));
                RenderDevice* renderDevice = renderer.getDevice();
                renderDevice->deleteResourceId(resource);
            }
        }

        void Buffer::init(Usage newUsage, uint32_t newFlags, uint32_t newSize)
        {
            usage = newUsage;
            flags = newFlags;
            size = newSize;

            renderer.addCommand(std::unique_ptr<Command>(new InitBufferCommand(resource,
                                                                               newUsage,
                                                                               newFlags,
                                                                               std::vector<uint8_t>(),
                                                                               newSize)));
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
                throw std::runtime_error("Invalid buffer data");

            usage = newUsage;
            flags = newFlags;
            size = newSize;

            renderer.addCommand(std::unique_ptr<Command>(new InitBufferCommand(resource,
                                                                               newUsage,
                                                                               newFlags,
                                                                               newData,
                                                                               newSize)));
        }

        void Buffer::setData(const void* newData, uint32_t newSize)
        {
            renderer.addCommand(std::unique_ptr<Command>(new SetBufferDataCommand(resource,
                                                                                  std::vector<uint8_t>(static_cast<const uint8_t*>(newData),
                                                                                                       static_cast<const uint8_t*>(newData) + newSize))));
        }

        void Buffer::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Buffer::DYNAMIC))
                throw std::runtime_error("Buffer is not dynamic");

            if (newData.empty())
                throw std::runtime_error("Invalid buffer data");

            if (newData.size() > size) size = static_cast<uint32_t>(newData.size());

            renderer.addCommand(std::unique_ptr<Command>(new SetBufferDataCommand(resource,
                                                                                  newData)));
        }
    } // namespace graphics
} // namespace ouzel
