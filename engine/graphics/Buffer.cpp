// Ouzel by Elviss Strazdins

#include "Buffer.hpp"
#include "Graphics.hpp"
#include "GraphicsError.hpp"

namespace ouzel::graphics
{
    Buffer::Buffer(Graphics& initGraphics):
        graphics{&initGraphics},
        resource{*initGraphics.getDevice()}
    {
    }

    Buffer::Buffer(Graphics& initGraphics,
                   BufferType initType,
                   Flags initFlags,
                   std::uint32_t initSize):
        graphics{&initGraphics},
        resource{*initGraphics.getDevice()},
        type{initType},
        flags{initFlags},
        size{initSize}
    {
        initGraphics.addCommand(std::make_unique<InitBufferCommand>(resource,
                                                                    initType,
                                                                    initFlags,
                                                                    std::vector<std::uint8_t>(),
                                                                    initSize));
    }

    Buffer::Buffer(Graphics& initGraphics,
                   BufferType initType,
                   Flags initFlags,
                   const void* initData,
                   std::uint32_t initSize):
        graphics{&initGraphics},
        resource{*initGraphics.getDevice()},
        type{initType},
        flags{initFlags},
        size{initSize}
    {
        initGraphics.addCommand(std::make_unique<InitBufferCommand>(resource,
                                                                    initType,
                                                                    initFlags,
                                                                    std::vector<std::uint8_t>(static_cast<const std::uint8_t*>(initData),
                                                                                              static_cast<const std::uint8_t*>(initData) + initSize),
                                                                    initSize));
    }

    Buffer::Buffer(Graphics& initGraphics,
                   BufferType initType,
                   Flags initFlags,
                   const std::vector<std::uint8_t>& initData,
                   std::uint32_t initSize):
        graphics{&initGraphics},
        resource{*initGraphics.getDevice()},
        type{initType},
        flags{initFlags},
        size{initSize}
    {
        if (!initData.empty() && initSize != initData.size())
            throw Error{"Invalid buffer data"};

        initGraphics.addCommand(std::make_unique<InitBufferCommand>(resource,
                                                                    initType,
                                                                    initFlags,
                                                                    initData,
                                                                    initSize));
    }

    void Buffer::setData(const void* newData, std::uint32_t newSize)
    {
        if (resource)
            graphics->addCommand(std::make_unique<SetBufferDataCommand>(resource,
                                                                        std::vector<std::uint8_t>(static_cast<const std::uint8_t*>(newData),
                                                                                                  static_cast<const std::uint8_t*>(newData) + newSize)));
    }

    void Buffer::setData(const std::vector<std::uint8_t>& newData)
    {
        if ((flags & Flags::dynamic) != Flags::dynamic)
            throw Error{"Buffer is not dynamic"};

        if (newData.empty())
            throw Error{"Invalid buffer data"};

        if (newData.size() > size) size = static_cast<std::uint32_t>(newData.size());

        if (resource)
            graphics->addCommand(std::make_unique<SetBufferDataCommand>(resource, newData));
    }
}
