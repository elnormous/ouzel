// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BUFFER_HPP
#define OUZEL_GRAPHICS_BUFFER_HPP

#include <vector>
#include "RenderDevice.hpp"
#include "BufferType.hpp"
#include "Flags.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Buffer final
        {
        public:
            Buffer() = default;

            explicit Buffer(Renderer& initRenderer);
            Buffer(Renderer& initRenderer,
                   BufferType initType,
                   Flags initFlags,
                   std::uint32_t initSize = 0);
            Buffer(Renderer& initRenderer,
                   BufferType initType,
                   Flags initFlags,
                   const void* initData,
                   std::uint32_t initSize);
            Buffer(Renderer& initRenderer,
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
            Renderer* renderer = nullptr;
            RenderDevice::Resource resource;

            BufferType type;
            Flags flags = Flags::none;
            std::uint32_t size = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BUFFER_HPP
