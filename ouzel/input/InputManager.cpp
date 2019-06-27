// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif
#include <algorithm>
#include <stdexcept>
#include "InputManager.hpp"
#include "Gamepad.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Touchpad.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "math/MathUtils.hpp"

#if TARGET_OS_IOS
#  include "input/ios/InputSystemIOS.hpp"
#elif TARGET_OS_TV
#  include "input/tvos/InputSystemTVOS.hpp"
#elif TARGET_OS_MAC
#  include "input/macos/InputSystemMacOS.hpp"
#elif defined(__ANDROID__)
#  include "input/android/InputSystemAndroid.hpp"
#elif defined(__linux__)
#  include "input/linux/InputSystemLinux.hpp"
#elif defined(_WIN32)
#  include "input/windows/InputSystemWin.hpp"
#elif defined(__EMSCRIPTEN__)
#  include "input/emscripten/InputSystemEm.hpp"
#endif

namespace ouzel
{
    namespace input
    {
        InputManager::InputManager():
#if TARGET_OS_IOS
            inputSystem(new InputSystemIOS(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif TARGET_OS_TV
            inputSystem(new InputSystemTVOS(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif TARGET_OS_MAC
        inputSystem(new InputSystemMacOS(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif defined(__ANDROID__)
            inputSystem(new InputSystemAndroid(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif defined(__linux__)
            inputSystem(new InputSystemLinux(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif defined(_WIN32)
            inputSystem(new InputSystemWin(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif defined(__EMSCRIPTEN__)
            inputSystem(new InputSystemEm(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#else
            inputSystem(new InputSystem(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#endif
        {
        }

        void InputManager::update()
        {
            std::pair<std::promise<bool>, InputSystem::Event> p;

            for (;;)
            {
                std::unique_lock<std::mutex> lock(eventQueueMutex);
                if (eventQueue.empty()) break;

                p = std::move(eventQueue.front());
                eventQueue.pop();
                lock.unlock();

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
        }

        bool InputManager::handleEvent(const InputSystem::Event& event)
        {
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
                            controllerMap.insert(std::make_pair(event.deviceId,
                                                                std::unique_ptr<Gamepad>(connectEvent->gamepad = new Gamepad(*this, event.deviceId))));
                            controllers.push_back(connectEvent->gamepad);
                            return engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                        }
                        case Controller::Type::KEYBOARD:
                        {
                            std::unique_ptr<KeyboardEvent> connectEvent(new KeyboardEvent());
                            connectEvent->type = Event::Type::KEYBOARD_CONNECT;
                            controllerMap.insert(std::make_pair(event.deviceId,
                                                                std::unique_ptr<Keyboard>(connectEvent->keyboard = new Keyboard(*this, event.deviceId))));
                            if (!keyboard) keyboard = connectEvent->keyboard;
                            controllers.push_back(connectEvent->keyboard);
                            return engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                        }
                        case Controller::Type::MOUSE:
                        {
                            std::unique_ptr<MouseEvent> connectEvent(new MouseEvent());
                            connectEvent->type = Event::Type::MOUSE_CONNECT;
                            controllerMap.insert(std::make_pair(event.deviceId,
                                                                std::unique_ptr<Mouse>(connectEvent->mouse = new Mouse(*this, event.deviceId))));
                            if (!mouse) mouse = connectEvent->mouse;
                            controllers.push_back(connectEvent->mouse);
                            return engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                        }
                        case Controller::Type::TOUCHPAD:
                        {
                            std::unique_ptr<TouchEvent> connectEvent(new TouchEvent());
                            connectEvent->type = Event::Type::TOUCHPAD_CONNECT;
                            controllerMap.insert(std::make_pair(event.deviceId,
                                                                std::unique_ptr<Touchpad>(connectEvent->touchpad = new Touchpad(*this, event.deviceId, event.screen))));
                            if (!touchpad) touchpad = connectEvent->touchpad;
                            controllers.push_back(connectEvent->touchpad);
                            return engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                        }
                        default: throw std::runtime_error("Invalid controller type");
                    }
                    break;
                }
                case InputSystem::Event::Type::DEVICE_DISCONNECT:
                {
                    bool handled = false;

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
                            default: throw std::runtime_error("Invalid controller type");
                        }

                        controllerMap.erase(i);
                    }

                    return handled;
                }
                case InputSystem::Event::Type::DEVICE_DISCOVERY_COMPLETE:
                {
                    discovering = false;
                    return true;
                }
                case InputSystem::Event::Type::GAMEPAD_BUTTON_CHANGE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Gamepad* gamepad = static_cast<Gamepad*>(i->second.get());
                        return gamepad->handleButtonValueChange(event.gamepadButton, event.pressed, event.value);
                    }
                    break;
                }
                case InputSystem::Event::Type::KEY_PRESS:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Keyboard* keyboardController = static_cast<Keyboard*>(i->second.get());
                        return keyboardController->handleKeyPress(event.keyboardKey);
                    }
                    break;
                }
                case InputSystem::Event::Type::KEY_RELEASE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Keyboard* keyboardController = static_cast<Keyboard*>(i->second.get());
                        return keyboardController->handleKeyRelease(event.keyboardKey);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_PRESS:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        return mouseController->handleButtonPress(event.mouseButton, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_RELEASE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        return mouseController->handleButtonRelease(event.mouseButton, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_SCROLL:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        return mouseController->handleScroll(event.scroll, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_MOVE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        return mouseController->handleMove(event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_RELATIVE_MOVE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        return mouseController->handleRelativeMove(event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::MOUSE_LOCK_CHANGED:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Mouse* mouseController = static_cast<Mouse*>(i->second.get());
                        return mouseController->handleCursorLockChange(event.locked);
                    }
                    break;
                }
                case InputSystem::Event::Type::TOUCH_BEGIN:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                        return touchpadController->handleTouchBegin(event.touchId, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::TOUCH_MOVE:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                        return touchpadController->handleTouchMove(event.touchId, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::TOUCH_END:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                        return touchpadController->handleTouchEnd(event.touchId, event.position);
                    }
                    break;
                }
                case InputSystem::Event::Type::TOUCH_CANCEL:
                {
                    auto i = controllerMap.find(event.deviceId);
                    if (i != controllerMap.end())
                    {
                        Touchpad* touchpadController = static_cast<Touchpad*>(i->second.get());
                        return touchpadController->handleTouchCancel(event.touchId, event.position);
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Unhandled event");
            }

            return false;
        }

        void InputManager::startDeviceDiscovery()
        {
            discovering = true;

            InputSystem::Command command(InputSystem::Command::Type::START_DEVICE_DISCOVERY);
            inputSystem->addCommand(command);
        }

        void InputManager::stopDeviceDiscovery()
        {
            discovering = false;

            InputSystem::Command command(InputSystem::Command::Type::STOP_DEVICE_DISCOVERY);
            inputSystem->addCommand(command);
        }

        void InputManager::showVirtualKeyboard()
        {
            InputSystem::Command command(InputSystem::Command::Type::SHOW_VIRTUAL_KEYBOARD);
            inputSystem->addCommand(command);
        }

        void InputManager::hideVirtualKeyboard()
        {
            InputSystem::Command command(InputSystem::Command::Type::HIDE_VIRTUAL_KEYBOARD);
            inputSystem->addCommand(command);
        }
    } // namespace input
} // namespace ouzel
