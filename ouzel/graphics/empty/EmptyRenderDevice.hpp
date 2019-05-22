// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP
#define OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP

#include "graphics/RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        class EmptyRenderDevice final: public RenderDevice
        {
        public:
            EmptyRenderDevice(const std::function<void(const Event&)>& initCallback);

        private:
            void process() final;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP
