// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "core/Setup.h"
#include "InputManager.hpp"
#include "Gamepad.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Touchpad.hpp"
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
            inputSystem(new InputSystemMacOS(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif OUZEL_PLATFORM_IOS
            inputSystem(new InputSystemIOS(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif OUZEL_PLATFORM_TVOS
            inputSystem(new InputSystemTVOS(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif OUZEL_PLATFORM_ANDROID
            inputSystem(new InputSystemAndroid(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif OUZEL_PLATFORM_LINUX
            inputSystem(new InputSystemLinux(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif OUZEL_PLATFORM_WINDOWS
            inputSystem(new InputSystemWin(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif OUZEL_PLATFORM_EMSCRIPTEN
            inputSystem(new InputSystemEm(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#else
            inputSystem(new InputSystem(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#endif
        {
        }

        void InputManager::update()
        {
            for (;;)
            {
                std::pair<std::promise<bool>, InputSystem::Event> p;
                {
                    std::unique_lock<std::mutex> lock(eventQueueMutex);
                    if (eventQueue.empty()) break;

                    p = std::move(eventQueue.front());
                    eventQueue.pop();
                }

                p.first.set_value(handleEvent(p.second));
            }
        }

        std::future<bool> InputManager::eventCallback(const InputSystem::Event& event)
        {
            std::pair<std::promise<bool>, InputSystem::Event> p(std::promise<bool>(), event);
            std::future<bool> f = p.first.get_future();

            std::unique_lock<std::mutex> lock(eventQueueMutex);
            eventQueue.push(std::move(p));

            return f;
        };

        bool InputManager::handleEvent(const InputSystem::Event& event)
        {
            bool handled = false;

            switch (event.type)
            {
                case InputSystem::Event::Type::DEVICE_CONNECT:
                {
                    switch (event.deviceType)
                    {
                        case Controller::Type::GAMEPAD:
                        {
                            std::unique_ptr<GamepadEvent> connectEvent(new GamepadEvent());
                            connectEvent->type = Event::Type::GAMEPAD_CONNECT;
                            std::unique_ptr<Gamepad> gamepadController(new Gamepad(*this, event.deviceId));
                            connectEvent->gamepad = gamepadController.get();
                            controllers.push_back(gamepadController.get());
                            controllerMap.insert(std::make_pair(event.deviceId, std::move(gamepadController)));
                            handled = engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                            break;
                        }
                        case Controller::Type::KEYBOARD:
                        {
                            std::unique_ptr<KeyboardEvent> connectEvent(new KeyboardEvent());
                            connectEvent->type = Event::Type::KEYBOARD_CONNECT;
                            std::unique_ptr<Keyboard> keyboardController(new Keyboard(*this, event.deviceId));
                            if (!keyboard) keyboard = keyboardController.get();
                            connectEvent->keyboard = keyboardController.get();
                            controllers.push_back(keyboardController.get());
                            controllerMap.insert(std::make_pair(event.deviceId, std::move(keyboardController)));
                            handled = engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                            break;
                        }
                        case Controller::Type::MOUSE:
                        {
                            std::unique_ptr<MouseEvent> connectEvent(new MouseEvent());
                            connectEvent->type = Event::Type::MOUSE_CONNECT;
                            std::unique_ptr<Mouse> mouseController(new Mouse(*this, event.deviceId));
                            connectEvent->mouse = mouseController.get();
                            if (!mouse) mouse = mouseController.get();
                            controllers.push_back(mouseController.get());
                            controllerMap.insert(std::make_pair(event.deviceId, std::move(mouseController)));
                            handled = engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                            break;
                        }
                        case Controller::Type::TOUCHPAD:
                        {
                            std::unique_ptr<TouchEvent> connectEvent(new TouchEvent());
                            connectEvent->type = Event::Type::TOUCHPAD_CONNECT;
                            std::unique_ptr<Touchpad> touchpadController(new Touchpad(*this, event.deviceId));
                            connectEvent->touchpad = touchpadController.get();
                            if (!touchpad) touchpad = touchpadController.get();
                            controllers.push_back(touchpadController.get());
                            controllerMap.insert(std::make_pair(event.deviceId, std::move(touchpadController)));
                            handled = engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                            break;
                        }
                    }
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

                        switch (i->second->getType())
                        {
                            case Controller::Type::GAMEPAD:
                            {
                                std::unique_ptr<GamepadEvent> disconnectEvent(new GamepadEvent());
                                disconnectEvent->type = Event::Type::GAMEPAD_DISCONNECT;
                                disconnectEvent->gamepad = static_cast<Gamepad*>(i->second.get());
                                handled = engine->getEventDispatcher().dispatchEvent(std::move(disconnectEvent));
                                break;
                            }
                            case Controller::Type::KEYBOARD:
                            {
                                std::unique_ptr<KeyboardEvent> disconnectEvent(new KeyboardEvent());
                                disconnectEvent->type = Event::Type::KEYBOARD_DISCONNECT;
                                disconnectEvent->keyboard = static_cast<Keyboard*>(i->second.get());
                                keyboard = nullptr;
                                for (Controller* controller : controllers)
                                    if (controller->getType() == Controller::Type::KEYBOARD)
                                        keyboard = static_cast<Keyboard*>(controller);
                                handled = engine->getEventDispatcher().dispatchEvent(std::move(disconnectEvent));
                                break;
                            }
                            case Controller::Type::MOUSE:
                            {
                                std::unique_ptr<MouseEvent> disconnectEvent(new MouseEvent());
                                disconnectEvent->type = Event::Type::MOUSE_DISCONNECT;
                                disconnectEvent->mouse = static_cast<Mouse*>(i->second.get());
                                mouse = nullptr;
                                for (Controller* controller : controllers)
                                    if (controller->getType() == Controller::Type::MOUSE)
                                        mouse = static_cast<Mouse*>(controller);
                                handled = engine->getEventDispatcher().dispatchEvent(std::move(disconnectEvent));
                                break;
                            }
                            case Controller::Type::TOUCHPAD:
                            {
                                std::unique_ptr<TouchEvent> disconnectEvent(new TouchEvent());
                                disconnectEvent->type = Event::Type::TOUCHPAD_DISCONNECT;
                                disconnectEvent->touchpad = static_cast<Touchpad*>(i->second.get());
                                touchpad = nullptr;
                                for (Controller* controller : controllers)
                                    if (controller->getType() == Controller::Type::TOUCHPAD)
                                        touchpad = static_cast<Touchpad*>(controller);
                                handled = engine->getEventDispatcher().dispatchEvent(std::move(disconnectEvent));
                                break;
                            }
                        }

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
                        handled = gamepad->handleButtonValueChange(event.gamepadButton, event.pressed, event.value);
                    }
                    break;
                }
                case InputSystem::Event::Type::KEY_PRESS:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Keyboard* keyboardController = static_cast<Keyboard*>(i->second.get());
                        handled = keyboardController->handleKeyPress(event.keyboardKey);
                    }
                    break;
                }
                case InputSystem::Event::Type::KEY_RELEASE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Keyboard* keyboardController = static_cast<Keyboard*>(i->second.get());
                        handled = keyboardController->handleKeyRelease(event.keyboardKey);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_PRESS:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        handled = mouseController->handleButtonPress(event.mouseButton, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_RELEASE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        handled = mouseController->handleButtonRelease(event.mouseButton, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_SCROLL:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        handled = mouseController->handleScroll(event.scroll, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_MOVE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        handled = mouseController->handleMove(event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_RELATIVE_MOVE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        handled = mouseController->handleRelativeMove(event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_LOCK_CHANGED:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        handled = mouseController->handleCursorLockChange(event.locked);
                    }
                    break;
                }
                case InputSystem::Event::Type::TOUCH_BEGIN:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                        handled = touchpadController->handleTouchBegin(event.touchId, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::TOUCH_MOVE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                        handled = touchpadController->handleTouchMove(event.touchId, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::TOUCH_END:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                        handled = touchpadController->handleTouchEnd(event.touchId, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::TOUCH_CANCEL:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                        handled = touchpadController->handleTouchCancel(event.touchId, event.position);
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Unhandled event");
                    break;
            }

            return handled;
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
