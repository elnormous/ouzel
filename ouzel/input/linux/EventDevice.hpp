// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_EVENTDEVICE_HPP
#define OUZEL_EVENTDEVICE_HPP

#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "input/Gamepad.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemLinux;
        class KeyboardDevice;
        class GamepadDevice;
        class MouseDevice;
        class TouchpadDevice;

        class EventDevice final
        {
        public:
            EventDevice(InputSystemLinux& inputSystem, const std::string& initFilename);
            ~EventDevice();

            EventDevice(const EventDevice& other) = delete;
            EventDevice& operator=(const EventDevice& other) = delete;
            EventDevice(EventDevice&& other) = delete;
            EventDevice& operator=(EventDevice&& other) = delete;

            void update();

            inline int getFd() const { return fd; }

        private:
            void handleAxisChange(int32_t oldValue, int32_t newValue,
                                  int32_t min, int32_t range,
                                  Gamepad::Button negativeButton, Gamepad::Button positiveButton);

            int fd = -1;
            std::string filename;
            std::string name;

            std::unique_ptr<KeyboardDevice> keyboardDevice;
            std::unique_ptr<GamepadDevice> gamepadDevice;
            std::unique_ptr<MouseDevice> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;

            Vector2 cursorPosition;
            struct Slot final
            {
                int32_t trackingId;
                int32_t positionX;
                int32_t positionY;
                int32_t pressure = 1;
                enum class Action
                {
                    NONE,
                    BEGIN,
                    END,
                    MOVE
                };
                Action action = Action::NONE;
            };
            std::vector<Slot> touchSlots;
            int32_t currentTouchSlot = 0;
            int32_t touchMinX;
            int32_t touchMaxX;
            int32_t touchMinY;
            int32_t touchMaxY;
            int32_t touchRangeX;
            int32_t touchRangeY;
            int32_t touchMinPressure = 0;
            int32_t touchMaxPressure = 1;

            int32_t hat0XValue = 0;
            int32_t hat0YValue = 0;

            struct Button final
            {
                Gamepad::Button button = Gamepad::Button::NONE;
                int32_t value = 0;
            };

            std::unordered_map<uint32_t, Button> buttons;

            struct Axis final
            {
                Gamepad::Axis axis = Gamepad::Axis::NONE;
                int32_t min = 0;
                int32_t max = 0;
                int32_t range = 0;
                int32_t value = 0;
                Gamepad::Button negativeButton = Gamepad::Button::NONE;
                Gamepad::Button positiveButton = Gamepad::Button::NONE;
            };

            std::unordered_map<uint32_t, Axis> axes;

            bool hasLeftTrigger = false;
            bool hasRightTrigger = false;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_EVENTDEVICE_HPP
