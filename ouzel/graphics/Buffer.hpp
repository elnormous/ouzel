// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BUFFER_HPP
#define OUZEL_GRAPHICS_BUFFER_HPP

#include <cstdint>
#include <vector>

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

            Buffer()
            {
            }

            explicit Buffer(Renderer& initRenderer);
            Buffer(Renderer& initRenderer, Usage newUsage, uint32_t newFlags, uint32_t newSize = 0);
            Buffer(Renderer& initRenderer, Usage newUsage, uint32_t newFlags, const void* newData, uint32_t newSize);
            Buffer(Renderer& initRenderer, Usage newUsage, uint32_t newFlags, const std::vector<uint8_t>& newData, uint32_t newSize);
            ~Buffer();

            Buffer(const Buffer&) = delete;
            Buffer& operator=(const Buffer&) = delete;

            Buffer(Buffer&& other):
                resource(other.resource),
                renderer(other.renderer),
                usage(other.usage),
                flags(other.flags),
                size(other.size)
            {
                other.renderer = nullptr;
                other.resource = 0;
            }

            Buffer& operator=(Buffer&& other)
            {
                if (&other != this)
                {
                    resource = other.resource;
                    renderer = other.renderer;
                    usage = other.usage;
                    flags = other.flags;
                    size = other.size;

                    other.renderer = nullptr;
                    other.resource = 0;
                }

                return *this;
            }

            void init(Usage newUsage, uint32_t newFlags, uint32_t newSize = 0);
            void init(Usage newUsage, uint32_t newFlags, const void* newData, uint32_t newSize);
            void init(Usage newUsage, uint32_t newFlags, const std::vector<uint8_t>& newData, uint32_t newSize);

            void setData(const void* newData, uint32_t newSize);
            void setData(const std::vector<uint8_t>& newData);

            inline uintptr_t getResource() const { return resource; }

            inline Usage getUsage() const { return usage; }
            inline uint32_t getFlags() const { return flags; }
            inline uint32_t getSize() const { return size; }

        private:
            Renderer* renderer = nullptr;
            uintptr_t resource = 0;

            Buffer::Usage usage;
            uint32_t flags = 0;
            uint32_t size = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BUFFER_HPP
