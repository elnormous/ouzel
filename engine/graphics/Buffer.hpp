// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BUFFER_HPP
#define OUZEL_GRAPHICS_BUFFER_HPP

#include <vector>
#include "graphics/RenderDevice.hpp"
#include "graphics/BufferType.hpp"
#include "graphics/Flags.hpp"

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
                   uint32_t initFlags,
                   uint32_t initSize = 0);
            Buffer(Renderer& initRenderer,
                   BufferType initType,
                   uint32_t initFlags,
                   const void* initData,
                   uint32_t initSize);
            Buffer(Renderer& initRenderer,
                   BufferType initType,
                   uint32_t initFlags,
                   const std::vector<uint8_t>& initData,
                   uint32_t initSize);

            void setData(const void* newData, uint32_t newSize);
            void setData(const std::vector<uint8_t>& newData);

            inline auto& getResource() const noexcept { return resource; }

            inline auto getType() const noexcept { return type; }
            inline auto getFlags() const noexcept { return flags; }
            inline auto getSize() const noexcept { return size; }

        private:
            Renderer* renderer = nullptr;
            RenderDevice::Resource resource;

            BufferType type;
            uint32_t flags = 0;
            uint32_t size = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BUFFER_HPP
