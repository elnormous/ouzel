// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BUFFER_HPP
#define OUZEL_GRAPHICS_BUFFER_HPP

#include <vector>
#include "graphics/RenderDevice.hpp"
#include "graphics/BufferType.hpp"
#include "graphics/Flags.hpp"
#include "utils/Inline.h"

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

            ALWAYSINLINE const RenderDevice::Resource& getResource() const { return resource; }

            ALWAYSINLINE BufferType getType() const { return type; }
            ALWAYSINLINE uint32_t getFlags() const { return flags; }
            ALWAYSINLINE uint32_t getSize() const { return size; }

        private:
            Renderer& renderer;
            RenderDevice::Resource resource;

            BufferType type;
            uint32_t flags = 0;
            uint32_t size = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BUFFER_HPP
