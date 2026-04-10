#ifndef FLUXION_INPUT_GAMEPADDEVICEEM_HPP
#define FLUXION_INPUT_GAMEPADDEVICEEM_HPP

#include "../GamepadDevice.hpp"
#include "../Gamepad.hpp"

namespace fluxion::input::emscripten
{
    class InputSystem;

    class GamepadDevice final: public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem,
                      DeviceId initId,
                      long initIndex);

        void update();

        auto getIndex() const noexcept { return index; }

    private:
        void handleThumbAxisChange(double oldValue, double newValue,
                                   Gamepad::Button negativeButton, Gamepad::Button positiveButton);

        long index = 0;
        double axis[64]{};
        double analogButton[64]{};
    };
}

#endif // FLUXION_INPUT_GAMEPADDEVICEEM_HPP
