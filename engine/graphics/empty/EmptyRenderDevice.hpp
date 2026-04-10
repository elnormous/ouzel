#ifndef FLUXION_GRAPHICS_EMPTYRENDERDEVICE_HPP
#define FLUXION_GRAPHICS_EMPTYRENDERDEVICE_HPP

#include "../RenderDevice.hpp"

namespace fluxion::graphics::empty
{
    class RenderDevice final: public graphics::RenderDevice
    {
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow):
            graphics::RenderDevice{Driver::empty, settings, initWindow}
        {
        }

        void start() final {}

    private:
        void process() final {}
    };
}

#endif // FLUXION_GRAPHICS_EMPTYRENDERDEVICE_HPP
