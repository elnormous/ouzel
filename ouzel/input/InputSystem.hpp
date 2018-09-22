// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <mutex>
#include <queue>
#include <vector>
#include "input/Gamepad.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputDevice;

        class InputSystem
        {
        public:
            struct Command
            {
                enum class Type
                {
                    START_DEVICE_DISCOVERY,
                    SET_ABSOLUTE_DPAD_VALUES,
                    SET_PLAYER_INDEX,
                    SET_VIBRATION
                };

                Type type;

                union
                {
                    bool absoluteDpadValues;
                    int32_t playerIndex;
                    float speed;
                };

                Gamepad::Motor motor;
            };

            struct Event
            {
                enum class Type
                {
                    GAMEPAD_CONNECT,
                    GAMEPAD_DISCONNECT,
                    GAMEPAD_BUTTON_CHANGE,

                    KEY_PRESS,
                    KEY_RELEASE,
                    KEY_REPEAT,

                    MOUSE_PRESS,
                    MOUSE_RELEASE,
                    MOUSE_SCROLL,
                    MOUSE_MOVE,

                    TOUCH_BEGIN,
                    TOUCH_MOVE,
                    TOUCH_END,
                    TOUCH_CANCEL
                };

                Type type;
                uint32_t deviceId = 0;
                union
                {
                    Keyboard::Key keyboardKey;
                    Mouse::Button mouseButton;
                    Gamepad::Button gamepadButton;
                    uint64_t touchId;
                };
                bool pressed = false;
                bool previousPressed = false;
                float value = 0.0F; // force for touch events
                float previousValue = 0.0F;
                Vector2 difference;
                Vector2 position;
                Vector2 scroll;
                uint32_t modifiers = 0;
            };

            InputSystem();

            std::vector<Event> getEvents() const;

        protected:
            void addEvent(const Event& event);

        private:
            mutable std::mutex eventQueueMutex;
            mutable std::queue<Event> eventQueue;

            mutable std::mutex commandQueueMutex;
            mutable std::queue<Event> commandQueue;
        };
    } // namespace input
} // namespace ouzel
