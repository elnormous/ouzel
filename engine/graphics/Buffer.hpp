// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_BUFFER_HPP
#define OUZEL_GRAPHICS_BUFFER_HPP

#include <vector>
#include "RenderDevice.hpp"
#include "BufferType.hpp"
#include "Flags.hpp"

namespace ouzel::graphics
{
    class Graphics;

    class Buffer final
    {
    public:
        Buffer() = default;

        explicit Buffer(Graphics& initGraphics);
        Buffer(Graphics& initGraphics,
               BufferType initType,
               Flags initFlags,
               std::uint32_t initSize = 0);
        Buffer(Graphics& initGraphics,
               BufferType initType,
               Flags initFlags,
               const void* initData,
               std::uint32_t initSize);
        Buffer(Graphics& initGraphics,
               BufferType initType,
               Flags initFlags,
               const std::vector<std::uint8_t>& initData,
               std::uint32_t initSize);

        void setData(const void* newData, std::uint32_t newSize);
        void setData(const std::vector<std::uint8_t>& newData);

        auto& getResource() const noexcept { return resource; }

        auto getType() const noexcept { return type; }
        auto getFlags() const noexcept { return flags; }
        auto getSize() const noexcept { return size; }

    private:
        Graphics* graphics = nullptr;
        RenderDevice::Resource resource;

        BufferType type;
        Flags flags = Flags::none;
        std::uint32_t size = 0;
    };
}

#endif // OUZEL_GRAPHICS_BUFFER_HPP
