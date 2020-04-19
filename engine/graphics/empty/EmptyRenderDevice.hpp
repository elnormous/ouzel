// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP
#define OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP

#include "../RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace empty
        {
            class RenderDevice final: public graphics::RenderDevice
            {
            public:
                explicit RenderDevice(const std::function<void(const Event&)>& initCallback):
                    graphics::RenderDevice(Driver::Empty, initCallback)
                {
                }

            private:
                void process() final {}
            };
        } // namespace empty
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP
