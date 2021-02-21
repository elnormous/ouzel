// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

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
#include "../core/Engine.hpp"
#include "../events/EventDispatcher.hpp"
#include "../math/MathUtils.hpp"

#if TARGET_OS_IOS
#  include "ios/InputSystemIOS.hpp"
#elif TARGET_OS_TV
#  include "tvos/InputSystemTVOS.hpp"
#elif TARGET_OS_MAC
#  include "macos/InputSystemMacOS.hpp"
#elif defined(__ANDROID__)
#  include "android/InputSystemAndroid.hpp"
#elif defined(__linux__)
#  include "linux/InputSystemLinux.hpp"
#elif defined(_WIN32)
#  include "windows/InputSystemWin.hpp"
#elif defined(__EMSCRIPTEN__)
#  include "emscripten/InputSystemEm.hpp"
#endif

namespace ouzel::input
{
    InputManager::InputManager():
#if TARGET_OS_IOS
        inputSystem(std::make_unique<ios::InputSystem>(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif TARGET_OS_TV
        inputSystem(std::make_unique<tvos::InputSystem>(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif TARGET_OS_MAC
        inputSystem(std::make_unique<macos::InputSystem>(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif defined(__ANDROID__)
        inputSystem(std::make_unique<android::InputSystem>(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif defined(__linux__)
        inputSystem(std::make_unique<linux::InputSystem>(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif defined(_WIN32)
        inputSystem(std::make_unique<windows::InputSystem>(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#elif defined(__EMSCRIPTEN__)
        inputSystem(std::make_unique<emscripten::InputSystem>(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#else
        inputSystem(std::make_unique<InputSystem>(std::bind(&InputManager::eventCallback, this, std::placeholders::_1)))
#endif
    {
    }

    void InputManager::update()
    {
        std::pair<std::promise<bool>, InputSystem::Event> p;

        for (;;)
        {
            std::unique_lock lock(eventQueueMutex);
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

        std::lock_guard lock(eventQueueMutex);
        eventQueue.push(std::move(p));

        return f;
    }

    bool InputManager::handleEvent(const InputSystem::Event& event)
    {
        switch (event.type)
        {
            case InputSystem::Event::Type::deviceConnect:
            {
                switch (event.deviceType)
                {
                    case Controller::Type::gamepad:
                    {
                        auto controller = std::make_unique<Gamepad>(*this, event.deviceId);
                        controllers.push_back(controller.get());

                        auto connectEvent = std::make_unique<GamepadEvent>();
                        connectEvent->type = Event::Type::gamepadConnect;
                        connectEvent->gamepad = controller.get();

                        controllerMap.insert(std::make_pair(event.deviceId, std::move(controller)));
                        return engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                    }
                    case Controller::Type::keyboard:
                    {
                        auto controller = std::make_unique<Keyboard>(*this, event.deviceId);
                        controllers.push_back(controller.get());
                        if (!keyboard) keyboard = controller.get();

                        auto connectEvent = std::make_unique<KeyboardEvent>();
                        connectEvent->type = Event::Type::keyboardConnect;
                        connectEvent->keyboard = controller.get();

                        controllerMap.insert(std::make_pair(event.deviceId, std::move(controller)));
                        return engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                    }
                    case Controller::Type::mouse:
                    {
                        auto controller = std::make_unique<Mouse>(*this, event.deviceId);
                        controllers.push_back(controller.get());
                        if (!mouse) mouse = controller.get();

                        auto connectEvent = std::make_unique<MouseEvent>();
                        connectEvent->type = Event::Type::mouseConnect;
                        connectEvent->mouse = controller.get();

                        controllerMap.insert(std::make_pair(event.deviceId, std::move(controller)));
                        return engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                    }
                    case Controller::Type::touchpad:
                    {
                        auto controller = std::make_unique<Touchpad>(*this, event.deviceId, event.screen);
                        controllers.push_back(controller.get());
                        if (!touchpad) touchpad = controller.get();

                        auto connectEvent = std::make_unique<TouchEvent>();
                        connectEvent->type = Event::Type::touchpadConnect;
                        connectEvent->touchpad = controller.get();

                        controllerMap.insert(std::make_pair(event.deviceId, std::move(controller)));
                        return engine->getEventDispatcher().dispatchEvent(std::move(connectEvent));
                    }
                    default: throw std::runtime_error("Invalid controller type");
                }
                break;
            }
            case InputSystem::Event::Type::deviceDisconnect:
            {
                bool handled = false;

                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    const auto controllerIterator = std::find(controllers.begin(), controllers.end(), i->second.get());
                    if (controllerIterator != controllers.end())
                        controllers.erase(controllerIterator);

                    switch (i->second->getType())
                    {
                        case Controller::Type::gamepad:
                        {
                            auto disconnectEvent = std::make_unique<GamepadEvent>();
                            disconnectEvent->type = Event::Type::gamepadDisconnect;
                            disconnectEvent->gamepad = static_cast<Gamepad*>(i->second.get());
                            handled = engine->getEventDispatcher().dispatchEvent(std::move(disconnectEvent));
                            break;
                        }
                        case Controller::Type::keyboard:
                        {
                            auto disconnectEvent = std::make_unique<KeyboardEvent>();
                            disconnectEvent->type = Event::Type::keyboardDisconnect;
                            disconnectEvent->keyboard = static_cast<Keyboard*>(i->second.get());
                            keyboard = nullptr;
                            for (Controller* controller : controllers)
                                if (controller->getType() == Controller::Type::keyboard)
                                    keyboard = static_cast<Keyboard*>(controller);
                            handled = engine->getEventDispatcher().dispatchEvent(std::move(disconnectEvent));
                            break;
                        }
                        case Controller::Type::mouse:
                        {
                            auto disconnectEvent = std::make_unique<MouseEvent>();
                            disconnectEvent->type = Event::Type::mouseDisconnect;
                            disconnectEvent->mouse = static_cast<Mouse*>(i->second.get());
                            mouse = nullptr;
                            for (Controller* controller : controllers)
                                if (controller->getType() == Controller::Type::mouse)
                                    mouse = static_cast<Mouse*>(controller);
                            handled = engine->getEventDispatcher().dispatchEvent(std::move(disconnectEvent));
                            break;
                        }
                        case Controller::Type::touchpad:
                        {
                            auto disconnectEvent = std::make_unique<TouchEvent>();
                            disconnectEvent->type = Event::Type::touchpadDisconnect;
                            disconnectEvent->touchpad = static_cast<Touchpad*>(i->second.get());
                            touchpad = nullptr;
                            for (Controller* controller : controllers)
                                if (controller->getType() == Controller::Type::touchpad)
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
            case InputSystem::Event::Type::deviceDiscoveryComplete:
            {
                discovering = false;
                return true;
            }
            case InputSystem::Event::Type::gamepadButtonChange:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto gamepad = static_cast<Gamepad*>(i->second.get());
                    return gamepad->handleButtonValueChange(event.gamepadButton, event.pressed, event.value);
                }
                break;
            }
            case InputSystem::Event::Type::keyboardKeyPress:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto keyboardController = static_cast<Keyboard*>(i->second.get());
                    return keyboardController->handleKeyPress(event.keyboardKey);
                }
                break;
            }
            case InputSystem::Event::Type::keyboardKeyRelease:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto keyboardController = static_cast<Keyboard*>(i->second.get());
                    return keyboardController->handleKeyRelease(event.keyboardKey);
                }
                break;
            }
            case InputSystem::Event::Type::mousePress:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto mouseController = static_cast<Mouse*>(i->second.get());
                    return mouseController->handleButtonPress(event.mouseButton, event.position);
                }
                break;
            }
            case InputSystem::Event::Type::mouseRelease:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto mouseController = static_cast<Mouse*>(i->second.get());
                    return mouseController->handleButtonRelease(event.mouseButton, event.position);
                }
                break;
            }
            case InputSystem::Event::Type::mouseScroll:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto mouseController = static_cast<Mouse*>(i->second.get());
                    return mouseController->handleScroll(event.scroll, event.position);
                }
                break;
            }
            case InputSystem::Event::Type::mouseMove:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto mouseController = static_cast<Mouse*>(i->second.get());
                    return mouseController->handleMove(event.position);
                }
                break;
            }
            case InputSystem::Event::Type::mouseRelativeMove:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto mouseController = static_cast<Mouse*>(i->second.get());
                    return mouseController->handleRelativeMove(event.position);
                }
                break;
            }
            case InputSystem::Event::Type::mouseLockChanged:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto mouseController = static_cast<Mouse*>(i->second.get());
                    return mouseController->handleCursorLockChange(event.locked);
                }
                break;
            }
            case InputSystem::Event::Type::touchBegin:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto touchpadController = static_cast<Touchpad*>(i->second.get());
                    return touchpadController->handleTouchBegin(event.touchId, event.position);
                }
                break;
            }
            case InputSystem::Event::Type::touchMove:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto touchpadController = static_cast<Touchpad*>(i->second.get());
                    return touchpadController->handleTouchMove(event.touchId, event.position);
                }
                break;
            }
            case InputSystem::Event::Type::touchEnd:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto touchpadController = static_cast<Touchpad*>(i->second.get());
                    return touchpadController->handleTouchEnd(event.touchId, event.position);
                }
                break;
            }
            case InputSystem::Event::Type::touchCancel:
            {
                const auto i = controllerMap.find(event.deviceId);
                if (i != controllerMap.end())
                {
                    auto touchpadController = static_cast<Touchpad*>(i->second.get());
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

        InputSystem::Command command(InputSystem::Command::Type::startDeviceDiscovery);
        inputSystem->addCommand(command);
    }

    void InputManager::stopDeviceDiscovery()
    {
        discovering = false;

        InputSystem::Command command(InputSystem::Command::Type::stopDeviceDiscovery);
        inputSystem->addCommand(command);
    }

    void InputManager::showVirtualKeyboard()
    {
        InputSystem::Command command(InputSystem::Command::Type::showVirtualKeyboard);
        inputSystem->addCommand(command);
    }

    void InputManager::hideVirtualKeyboard()
    {
        InputSystem::Command command(InputSystem::Command::Type::hideVirtualKeyboard);
        inputSystem->addCommand(command);
    }
}
