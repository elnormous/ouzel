// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "core/Setup.h"
#include "InputManager.hpp"
#include "Gamepad.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Touchpad.hpp"
#include "NativeCursor.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "math/MathUtils.hpp"

#if OUZEL_PLATFORM_MACOS
#  include "input/macos/InputSystemMacOS.hpp"
#elif OUZEL_PLATFORM_IOS
#  include "input/ios/InputSystemIOS.hpp"
#elif OUZEL_PLATFORM_TVOS
#  include "input/tvos/InputSystemTVOS.hpp"
#elif OUZEL_PLATFORM_ANDROID
#  include "input/android/InputSystemAndroid.hpp"
#elif OUZEL_PLATFORM_LINUX
#  include "input/linux/InputSystemLinux.hpp"
#elif OUZEL_PLATFORM_WINDOWS
#  include "input/windows/InputSystemWin.hpp"
#elif OUZEL_PLATFORM_EMSCRIPTEN
#  include "input/emscripten/InputSystemEm.hpp"
#endif

namespace ouzel
{
    namespace input
    {
        InputManager::InputManager():
#if OUZEL_PLATFORM_MACOS
        inputSystem(new InputSystemMacOS())
#elif OUZEL_PLATFORM_IOS
        inputSystem(new InputSystemIOS())
#elif OUZEL_PLATFORM_TVOS
        inputSystem(new InputSystemTVOS())
#elif OUZEL_PLATFORM_ANDROID
        inputSystem(new InputSystemAndroid())
#elif OUZEL_PLATFORM_LINUX
        inputSystem(new InputSystemLinux())
#elif OUZEL_PLATFORM_WINDOWS
        inputSystem(new InputSystemWin())
#elif OUZEL_PLATFORM_EMSCRIPTEN
        inputSystem(new InputSystemEm())
#else
        inputSystem(new InputSystem())
#endif
        {
        }

        void InputManager::update()
        {
            std::vector<InputSystem::Event> events = inputSystem->getEvents();

            for (const InputSystem::Event& event : events)
            {
                switch (event.type)
                {
                    case InputSystem::Event::Type::DEVICE_CONNECT:
                    {
                        Event connectEvent;

                        switch (event.deviceType)
                        {
                            case Controller::Type::GAMEPAD:
                            {
                                connectEvent.type = Event::Type::GAMEPAD_CONNECT;
                                std::unique_ptr<Gamepad> gamepadController(new Gamepad(*this, event.deviceId));
                                connectEvent.gamepadEvent.gamepad = gamepadController.get();
                                controllers.push_back(gamepadController.get());
                                controllerMap.insert(std::make_pair(event.deviceId, std::move(gamepadController)));
                                break;
                            }
                            case Controller::Type::KEYBOARD:
                            {
                                connectEvent.type = Event::Type::KEYBOARD_CONNECT;
                                std::unique_ptr<Keyboard> keyboardController(new Keyboard(*this, event.deviceId));
                                if (!keyboard) keyboard = keyboardController.get();
                                connectEvent.keyboardEvent.keyboard = keyboardController.get();
                                controllers.push_back(keyboardController.get());
                                controllerMap.insert(std::make_pair(event.deviceId, std::move(keyboardController)));
                                break;
                            }
                            case Controller::Type::MOUSE:
                            {
                                connectEvent.type = Event::Type::MOUSE_CONNECT;
                                std::unique_ptr<Mouse> mouseController(new Mouse(*this, event.deviceId));
                                connectEvent.mouseEvent.mouse = mouseController.get();
                                if (!mouse) mouse = mouseController.get();
                                controllers.push_back(mouseController.get());
                                controllerMap.insert(std::make_pair(event.deviceId, std::move(mouseController)));
                                break;
                            }
                            case Controller::Type::TOUCHPAD:
                            {
                                connectEvent.type = Event::Type::TOUCHPAD_CONNECT;
                                std::unique_ptr<Touchpad> touchpadController(new Touchpad(*this, event.deviceId));
                                connectEvent.touchEvent.touchpad = touchpadController.get();
                                if (!touchpad) touchpad = touchpadController.get();
                                controllers.push_back(touchpadController.get());
                                controllerMap.insert(std::make_pair(event.deviceId, std::move(touchpadController)));
                                break;
                            }
                        }

                        engine->getEventDispatcher().dispatchEvent(connectEvent);
                        break;
                    }
                    case InputSystem::Event::Type::DEVICE_DISCONNECT:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            auto controllerIterator = std::find(controllers.begin(), controllers.end(), i->second.get());
                            if (controllerIterator != controllers.end())
                                controllers.erase(controllerIterator);

                            Event disconnectEvent;

                            switch (i->second->getType())
                            {
                                case Controller::Type::GAMEPAD:
                                {
                                    disconnectEvent.type = Event::Type::GAMEPAD_DISCONNECT;
                                    disconnectEvent.gamepadEvent.gamepad = static_cast<Gamepad*>(i->second.get());
                                    break;
                                }
                                case Controller::Type::KEYBOARD:
                                {
                                    disconnectEvent.type = Event::Type::KEYBOARD_DISCONNECT;
                                    disconnectEvent.keyboardEvent.keyboard = static_cast<Keyboard*>(i->second.get());
                                    keyboard = nullptr;
                                    for (Controller* controller : controllers)
                                        if (controller->getType() == Controller::Type::KEYBOARD)
                                            keyboard = static_cast<Keyboard*>(controller);
                                    break;
                                }
                                case Controller::Type::MOUSE:
                                {
                                    disconnectEvent.type = Event::Type::MOUSE_DISCONNECT;
                                    disconnectEvent.mouseEvent.mouse = static_cast<Mouse*>(i->second.get());
                                    mouse = nullptr;
                                    for (Controller* controller : controllers)
                                        if (controller->getType() == Controller::Type::MOUSE)
                                            mouse = static_cast<Mouse*>(controller);
                                    break;
                                }
                                case Controller::Type::TOUCHPAD:
                                {
                                    disconnectEvent.type = Event::Type::TOUCHPAD_DISCONNECT;
                                    disconnectEvent.touchEvent.touchpad = static_cast<Touchpad*>(i->second.get());
                                    touchpad = nullptr;
                                    for (Controller* controller : controllers)
                                        if (controller->getType() == Controller::Type::TOUCHPAD)
                                            touchpad = static_cast<Touchpad*>(controller);
                                    break;
                                }
                            }

                            engine->getEventDispatcher().dispatchEvent(disconnectEvent);
                            controllerMap.erase(i);
                        }

                        break;
                    }
                    case InputSystem::Event::Type::DEVICE_DISCOVERY_COMPLETE:
                    {
                        discovering = false;
                        break;
                    }
                    case InputSystem::Event::Type::GAMEPAD_BUTTON_CHANGE:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Gamepad* gamepad = static_cast<Gamepad*>(i->second.get());
                            gamepad->handleButtonValueChange(event.gamepadButton, event.pressed, event.value);
                        }

                        break;
                    }
                    case InputSystem::Event::Type::KEY_PRESS:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Keyboard* keyboardController = static_cast<Keyboard*>(i->second.get());
                            keyboardController->handleKeyPress(event.keyboardKey);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::KEY_RELEASE:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Keyboard* keyboardController = static_cast<Keyboard*>(i->second.get());
                            keyboardController->handleKeyRelease(event.keyboardKey);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_PRESS:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                            mouseController->handleButtonPress(event.mouseButton, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_RELEASE:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                            mouseController->handleButtonRelease(event.mouseButton, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_SCROLL:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                            mouseController->handleScroll(event.scroll, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_MOVE:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                            mouseController->handleMove(event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_RELATIVE_MOVE:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                            mouseController->handleRelativeMove(event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_LOCK_CHANGED:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                            mouseController->handleCursorLockChange(event.locked);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::TOUCH_BEGIN:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                            touchpadController->handleTouchBegin(event.touchId, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::TOUCH_MOVE:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                            touchpadController->handleTouchMove(event.touchId, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::TOUCH_END:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                            touchpadController->handleTouchEnd(event.touchId, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::TOUCH_CANCEL:
                    {
                        auto i = controllerMap.find(event.deviceId);
                        if (i != controllerMap.end())
                        {
                            Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                            touchpadController->handleTouchCancel(event.touchId, event.position);
                        }
                        break;
                    }
                }
            }
        }

        void InputManager::startDeviceDiscovery()
        {
            discovering = true;

            InputSystem::Command command;
            command.type = InputSystem::Command::Type::START_DEVICE_DISCOVERY;
            inputSystem->addCommand(command);
        }

        void InputManager::stopDeviceDiscovery()
        {
            discovering = false;

            InputSystem::Command command;
            command.type = InputSystem::Command::Type::STOP_DEVICE_DISCOVERY;
            inputSystem->addCommand(command);
        }

        void InputManager::showVirtualKeyboard()
        {
            InputSystem::Command command;
            command.type = InputSystem::Command::Type::SHOW_VIRTUAL_KEYBOARD;
            inputSystem->addCommand(command);
        }

        void InputManager::hideVirtualKeyboard()
        {
            InputSystem::Command command;
            command.type = InputSystem::Command::Type::HIDE_VIRTUAL_KEYBOARD;
            inputSystem->addCommand(command);
        }
    } // namespace input
} // namespace ouzel
