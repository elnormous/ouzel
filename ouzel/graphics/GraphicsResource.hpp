// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_GRAPHICSRESOURCE_HPP
#define OUZEL_GRAPHICS_GRAPHICSRESOURCE_HPP

#include <cstdint>
#include "utils/Inline.h"

namespace ouzel
{
    namespace graphics
    {
        class RenderDevice;

        class Resource final
        {
        public:
            Resource() = default;
            Resource(RenderDevice& initRendererDevice);
            ~Resource();

            Resource(const Resource&) = delete;
            Resource& operator=(const Resource&) = delete;

            Resource(Resource&& other):
                renderDevice(other.renderDevice),
                id(other.id)
            {
                other.renderDevice = nullptr;
                other.id = 0;
            }

            Resource& operator=(Resource&& other)
            {
                if (&other != this)
                {
                    renderDevice = other.renderDevice;
                    id = other.id;
                    other.renderDevice = nullptr;
                    other.id = 0;
                }

                return *this;
            }

            ALWAYSINLINE uintptr_t getId() const
            {
                return id;
            }

        private:
            RenderDevice* renderDevice = nullptr;
            uintptr_t id = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_GRAPHICSRESOURCE_HPP
