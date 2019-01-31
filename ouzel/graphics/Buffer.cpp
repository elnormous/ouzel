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
            resource(initRenderer)
        {
        }

        Buffer::Buffer(Renderer& initRenderer, Usage initUsage, uint32_t initFlags, uint32_t initSize):
            resource(initRenderer),
            usage(initUsage),
            flags(initFlags),
            size(initSize)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitBufferCommand(resource.getId(),
                                                                                   initUsage,
                                                                                   initFlags,
                                                                                   std::vector<uint8_t>(),
                                                                                   initSize)));
        }

        Buffer::Buffer(Renderer& initRenderer, Usage initUsage, uint32_t initFlags, const void* initData, uint32_t initSize):
            resource(initRenderer),
            usage(initUsage),
            flags(initFlags),
            size(initSize)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitBufferCommand(resource.getId(),
                                                                                   initUsage,
                                                                                   initFlags,
                                                                                   std::vector<uint8_t>(static_cast<const uint8_t*>(initData),
                                                                                                        static_cast<const uint8_t*>(initData) + initSize),
                                                                                   initSize)));
        }

        Buffer::Buffer(Renderer& initRenderer, Usage initUsage, uint32_t initFlags, const std::vector<uint8_t>& initData, uint32_t initSize):
            resource(initRenderer),
            usage(initUsage),
            flags(initFlags),
            size(initSize)
        {
            if (!initData.empty() && initSize != initData.size())
                throw std::runtime_error("Invalid buffer data");

            initRenderer.addCommand(std::unique_ptr<Command>(new InitBufferCommand(resource.getId(),
                                                                                   initUsage,
                                                                                   initFlags,
                                                                                   initData,
                                                                                   initSize)));
        }

        void Buffer::setData(const void* newData, uint32_t newSize)
        {
            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetBufferDataCommand(resource.getId(),
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

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetBufferDataCommand(resource.getId(),
                                                                                                     newData)));
        }
    } // namespace graphics
} // namespace ouzel
