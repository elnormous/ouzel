// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BUFFER_HPP
#define OUZEL_GRAPHICS_BUFFER_HPP

#include <vector>
#include "graphics/GraphicsResource.hpp"
#include "utils/Inline.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Buffer final
        {
        public:
            enum Flags
            {
                DYNAMIC = 0x01
            };

            enum class Usage
            {
                NONE,
                INDEX,
                VERTEX
            };

            Buffer() = default;

            explicit Buffer(Renderer& initRenderer);
            Buffer(Renderer& initRenderer,
                   Usage newUsage,
                   uint32_t newFlags,
                   uint32_t newSize = 0);
            Buffer(Renderer& initRenderer,
                   Usage newUsage,
                   uint32_t newFlags,
                   const void* newData,
                   uint32_t newSize);
            Buffer(Renderer& initRenderer,
                   Usage newUsage,
                   uint32_t newFlags,
                   const std::vector<uint8_t>& newData,
                   uint32_t newSize);

            void setData(const void* newData, uint32_t newSize);
            void setData(const std::vector<uint8_t>& newData);

            ALWAYSINLINE uintptr_t getResource() const { return resource.getId(); }

            ALWAYSINLINE Usage getUsage() const { return usage; }
            ALWAYSINLINE uint32_t getFlags() const { return flags; }
            ALWAYSINLINE uint32_t getSize() const { return size; }

        private:
            Renderer& renderer;
            Resource resource;

            Buffer::Usage usage;
            uint32_t flags = 0;
            uint32_t size = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BUFFER_HPP
