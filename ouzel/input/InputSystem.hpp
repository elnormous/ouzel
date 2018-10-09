// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>
#include "input/GamepadDevice.hpp"
#include "input/KeyboardDevice.hpp"
#include "input/MouseDevice.hpp"
#include "input/TouchpadDevice.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystem
        {
            friend InputDevice;
            friend GamepadDevice;
            friend KeyboardDevice;
            friend MouseDevice;
            friend TouchpadDevice;
        public:
            struct Command
            {
                enum class Type
                {
                    START_DEVICE_DISCOVERY,
                    STOP_DEVICE_DISCOVERY,
                    SET_ABSOLUTE_DPAD_VALUES,
                    SET_PLAYER_INDEX,
                    SET_VIBRATION,
                    SET_POSITION,
                    SET_CURSOR,
                    SET_CURSOR_VISIBLE,
                    SET_CURSOR_LOCKED,
                    SHOW_VIRTUAL_KEYBOARD,
                    HIDE_VIRTUAL_KEYBOARD
                };

                Type type;
                uint32_t deviceId = 0;

                union
                {
                    bool absoluteDPadValues;
                    int32_t playerIndex;
                    float speed;
                    bool visible;
                    bool locked;
                };

                Gamepad::Motor motor;
                Vector2 position;
            };

            struct Event
            {
                enum class Type
                {
                    DEVICE_CONNECT,
                    DEVICE_DISCONNECT,
                    DEVICE_DISCOVERY_COMPLETE,

                    GAMEPAD_BUTTON_CHANGE,

                    KEY_PRESS,
                    KEY_RELEASE,

                    MOUSE_PRESS,
                    MOUSE_RELEASE,
                    MOUSE_SCROLL,
                    MOUSE_MOVE,
                    MOUSE_RELATIVE_MOVE,
                    MOUSE_LOCK_CHANGED,

                    TOUCH_BEGIN,
                    TOUCH_MOVE,
                    TOUCH_END,
                    TOUCH_CANCEL
                };

                Type type;
                Controller::Type deviceType;
                uint32_t deviceId = 0;
                union
                {
                    Keyboard::Key keyboardKey;
                    Mouse::Button mouseButton;
                    Gamepad::Button gamepadButton;
                    uint64_t touchId;
                };
                bool locked = false;
                bool pressed = false;
                bool previousPressed = false;
                float value = 0.0F; // force for touch events
                float previousValue = 0.0F;
                Vector2 difference;
                Vector2 position;
                Vector2 scroll;
                float force = 1.0F;
            };

            InputSystem();
            virtual ~InputSystem() {}

            void addCommand(const Command& command);
            virtual void executeCommand(const Command&) {}

            std::vector<Event> getEvents() const;

        protected:
            void postEvent(const Event& event);
            void addInputDevice(InputDevice& inputDevice);
            void removeInputDevice(const InputDevice& inputDevice);
            InputDevice* getInputDevice(uint32_t id);

            std::unordered_map<uint32_t, InputDevice*> inputDevices;

        private:
            mutable std::mutex eventQueueMutex;
            mutable std::queue<Event> eventQueue;
        };
    } // namespace input
} // namespace ouzel
