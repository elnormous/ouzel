// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP
#define OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP

#include "../RenderDevice.hpp"

namespace ouzel::graphics::empty
{
    class RenderDevice final: public graphics::RenderDevice
    {
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow,
                     const std::function<void(const Event&)>& initCallback):
            graphics::RenderDevice{Driver::empty, settings, initWindow, initCallback}
        {
        }

        void start() final {}

    private:
        void process() final {}
    };
}

#endif // OUZEL_GRAPHICS_EMPTYRENDERDEVICE_HPP
