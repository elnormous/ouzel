// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP
#define OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP

#include "graphics/RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace empty
        {
            class RenderDevice final: public graphics::RenderDevice
            {
            public:
                RenderDevice(const std::function<void(const Event&)>& initCallback);

            private:
                void process() final;
            };
        } // namespace empty
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP
