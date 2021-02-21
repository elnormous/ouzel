// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEM_HPP
#define OUZEL_INPUT_INPUTSYSTEM_HPP

#include <cstdint>
#include <future>
#include <set>
#include <unordered_map>
#include <vector>
#include "GamepadDevice.hpp"
#include "KeyboardDevice.hpp"
#include "MouseDevice.hpp"
#include "SystemCursor.hpp"
#include "TouchpadDevice.hpp"
#include "../graphics/PixelFormat.hpp"
#include "../math/Size.hpp"
#include "../math/Vector.hpp"

namespace ouzel::input
{
    class InputSystem
    {
        friend InputDevice;
        friend GamepadDevice;
        friend KeyboardDevice;
        friend MouseDevice;
        friend TouchpadDevice;
    public:
        class Command final
        {
        public:
            enum class Type
            {
                startDeviceDiscovery,
                stopDeviceDiscovery,
                setAbsoluteDpadValues,
                setPlayerIndex,
                setVibration,
                setPosition,
                initCursor,
                destroyCursor,
                setCursor,
                setCursorVisible,
                setCursorLocked,
                showVirtualKeyboard,
                hideVirtualKeyboard
            };

            Command() = default;
            explicit Command(Type initType): type(initType) {}

            Type type;
            DeviceId deviceId;

            union
            {
                bool absoluteDpadValues = false;
                std::int32_t playerIndex;
                float speed;
                bool visible;
                bool locked;
            };

            Gamepad::Motor motor;
            Vector2F position;
            std::size_t cursorResource;
            SystemCursor systemCursor;
            std::vector<std::uint8_t> data;
            Size2F size;
            graphics::PixelFormat pixelFormat;
            Vector2F hotSpot;
        };

        class Event final
        {
        public:
            enum class Type
            {
                none,

                deviceConnect,
                deviceDisconnect,
                deviceDiscoveryComplete,

                gamepadButtonChange,

                keyboardKeyPress,
                keyboardKeyRelease,

                mousePress,
                mouseRelease,
                mouseScroll,
                mouseMove,
                mouseRelativeMove,
                mouseLockChanged,

                touchBegin,
                touchMove,
                touchEnd,
                touchCancel
            };

            Event() = default;
            explicit Event(Type initType): type(initType) {}

            Type type = Type::none;
            Controller::Type deviceType = Controller::Type::none;
            DeviceId deviceId;
            union
            {
                Keyboard::Key keyboardKey;
                Mouse::Button mouseButton;
                Gamepad::Button gamepadButton;
                std::uint64_t touchId = 0;
            };
            bool screen = false;
            bool locked = false;
            bool pressed = false;
            bool previousPressed = false;
            float value = 0.0F; // force for touch events
            float previousValue = 0.0F;
            Vector2F difference;
            Vector2F position;
            Vector2F scroll;
            float force = 1.0F;
        };

        explicit InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback);
        virtual ~InputSystem() = default;

        void addCommand(const Command& command);

        auto getResourceId()
        {
            const auto i = deletedResourceIds.begin();

            if (i == deletedResourceIds.end())
                return ++lastResourceId; // zero is reserved for null resource
            else
            {
                std::size_t resourceId = *i;
                deletedResourceIds.erase(i);
                return resourceId;
            }
        }

        void deleteResourceId(std::size_t resourceId)
        {
            deletedResourceIds.insert(resourceId);
        }

    protected:
        std::future<bool> sendEvent(const Event& event);
        void addInputDevice(InputDevice& inputDevice);
        void removeInputDevice(const InputDevice& inputDevice);
        InputDevice* getInputDevice(DeviceId id);

    private:
        virtual void executeCommand(const Command&) {}

        std::function<std::future<bool>(const Event&)> callback;
        std::unordered_map<DeviceId, InputDevice*> inputDevices;

        std::size_t lastResourceId = 0;
        std::set<std::size_t> deletedResourceIds;
    };
}

#endif // OUZEL_INPUT_INPUTSYSTEM_HPP
