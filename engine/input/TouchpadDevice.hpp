#ifndef FLUXION_INPUT_TOUCHPADDEVICE_HPP
#define FLUXION_INPUT_TOUCHPADDEVICE_HPP

#include <future>
#include "InputDevice.hpp"
#include "../math/Vector.hpp"

namespace fluxion::input
{
    class TouchpadDevice: public InputDevice
    {
    public:
        TouchpadDevice(InputSystem& initInputSystem, DeviceId initId, bool screen);
        ~TouchpadDevice() override;

        std::future<bool> handleTouchBegin(std::uint64_t touchId, const math::Vector<float, 2>& position, float force = 1.0F);
        std::future<bool> handleTouchEnd(std::uint64_t touchId, const math::Vector<float, 2>& position, float force = 1.0F);
        std::future<bool> handleTouchMove(std::uint64_t touchId, const math::Vector<float, 2>& position, float force = 1.0F);
        std::future<bool> handleTouchCancel(std::uint64_t touchId, const math::Vector<float, 2>& position, float force = 1.0F);
    };
}

#endif // FLUXION_INPUT_TOUCHPADDEVICE_HPP
