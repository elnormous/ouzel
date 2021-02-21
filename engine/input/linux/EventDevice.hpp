// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_EVENTDEVICE_HPP
#define OUZEL_INPUT_EVENTDEVICE_HPP

#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../Gamepad.hpp"

namespace ouzel::input
{
    class GamepadDevice;
    class TouchpadDevice;
}

namespace ouzel::input::linux
{
    class InputSystem;
    class KeyboardDevice;
    class MouseDevice;

    class EventDevice final
    {
    public:
        EventDevice(InputSystem& inputSystem, const std::string& initFilename);
        ~EventDevice();

        EventDevice(const EventDevice& other) = delete;
        EventDevice& operator=(const EventDevice& other) = delete;
        EventDevice(EventDevice&& other) = delete;
        EventDevice& operator=(EventDevice&& other) = delete;

        void update();

        auto getFd() const noexcept { return fd; }

    private:
        void handleAxisChange(std::int32_t oldValue, std::int32_t newValue,
                              std::int32_t min, std::int32_t range,
                              Gamepad::Button negativeButton, Gamepad::Button positiveButton);

        int fd = -1;
        std::string filename;
        std::string name;

        std::unique_ptr<KeyboardDevice> keyboardDevice;
        std::unique_ptr<GamepadDevice> gamepadDevice;
        std::unique_ptr<MouseDevice> mouseDevice;
        std::unique_ptr<TouchpadDevice> touchpadDevice;

        struct Position final
        {
            std::int32_t x = 0;
            std::int32_t y = 0;
        };
        Position cursorPosition;
        struct Slot final
        {
            std::int32_t trackingId;
            std::int32_t positionX;
            std::int32_t positionY;
            std::int32_t pressure = 1;
            enum class Action
            {
                none,
                begin,
                end,
                move
            };
            Action action = Action::none;
        };
        std::vector<Slot> touchSlots;
        std::int32_t currentTouchSlot = 0;
        std::int32_t touchMinX;
        std::int32_t touchMaxX;
        std::int32_t touchMinY;
        std::int32_t touchMaxY;
        std::int32_t touchRangeX;
        std::int32_t touchRangeY;
        std::int32_t touchMinPressure = 0;
        std::int32_t touchMaxPressure = 1;

        std::int32_t hat0XValue = 0;
        std::int32_t hat0YValue = 0;

        struct Button final
        {
            Gamepad::Button button = Gamepad::Button::none;
            std::int32_t value = 0;
        };

        std::unordered_map<std::uint32_t, Button> buttons;

        struct Axis final
        {
            Gamepad::Axis axis = Gamepad::Axis::none;
            std::int32_t min = 0;
            std::int32_t max = 0;
            std::int32_t range = 0;
            std::int32_t value = 0;
            Gamepad::Button negativeButton = Gamepad::Button::none;
            Gamepad::Button positiveButton = Gamepad::Button::none;
        };

        std::unordered_map<std::uint32_t, Axis> axes;

        bool hasLeftTrigger = false;
        bool hasRightTrigger = false;

        std::uint8_t buffer[1024];
        std::size_t bufferSize = 0;
    };
}

#endif // OUZEL_INPUT_EVENTDEVICE_HPP
