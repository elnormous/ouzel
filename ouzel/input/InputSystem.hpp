// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <future>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>
#include "input/GamepadDevice.hpp"
#include "input/KeyboardDevice.hpp"
#include "input/MouseDevice.hpp"
#include "input/SystemCursor.hpp"
#include "input/TouchpadDevice.hpp"
#include "graphics/PixelFormat.hpp"
#include "math/Size2.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursor;

        class InputSystem
        {
            friend InputDevice;
            friend GamepadDevice;
            friend KeyboardDevice;
            friend MouseDevice;
            friend TouchpadDevice;
        public:
            struct Command final
            {
                enum class Type
                {
                    START_DEVICE_DISCOVERY,
                    STOP_DEVICE_DISCOVERY,
                    SET_ABSOLUTE_DPAD_VALUES,
                    SET_PLAYER_INDEX,
                    SET_VIBRATION,
                    SET_POSITION,
                    INIT_CURSOR,
                    DESTROY_CURSOR,
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
                uintptr_t cursorResource;
                SystemCursor systemCursor;
                std::vector<uint8_t> data;
                Size2 size;
                graphics::PixelFormat pixelFormat;
                Vector2 hotSpot;
            };

            struct Event final
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

            class EventHandler
            {
            public:
                virtual ~EventHandler() {}
                virtual bool handleEvent(const Event& event) = 0;
            };

            explicit InputSystem(EventHandler& initEventHandler);
            virtual ~InputSystem() {}

            void addCommand(const Command& command);
            virtual void executeCommand(const Command&) {}

            void dispatchEvents();

            uintptr_t getResourceId()
            {
                if (deletedResourceIds.empty())
                    return ++lastResourceId; // zero is reserved for null resource
                else
                {
                    uintptr_t resourceId = deletedResourceIds.front();
                    deletedResourceIds.pop();
                    return resourceId;
                }
            }

            void deleteResourceId(uintptr_t resourceId)
            {
                deletedResourceIds.push(resourceId);
            }

        protected:
            std::future<bool> postEvent(const Event& event);
            void addInputDevice(InputDevice& inputDevice);
            void removeInputDevice(const InputDevice& inputDevice);
            InputDevice* getInputDevice(uint32_t id);

        private:
            EventHandler& eventHandler;
            std::mutex eventQueueMutex;
            std::queue<std::pair<std::promise<bool>, Event>> eventQueue;
            std::unordered_map<uint32_t, InputDevice*> inputDevices;

            uintptr_t lastResourceId = 0;
            std::queue<uintptr_t> deletedResourceIds;
        };
    } // namespace input
} // namespace ouzel
