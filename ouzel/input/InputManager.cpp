// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "core/Setup.h"
#include "InputManager.hpp"
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

        InputManager::~InputManager()
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
                                std::unique_ptr<Gamepad> gamepad(new Gamepad(event.deviceId));
                                connectEvent.gamepadEvent.gamepad = gamepad.get();
                                controllers.insert(std::make_pair(event.deviceId, std::move(gamepad)));
                                break;
                            }
                            case Controller::Type::KEYBOARD:
                            {
                                connectEvent.type = Event::Type::KEYBOARD_CONNECT;
                                std::unique_ptr<Keyboard> keyboard(new Keyboard(event.deviceId));
                                connectEvent.keyboardEvent.keyboard = keyboard.get();
                                controllers.insert(std::make_pair(event.deviceId, std::move(keyboard)));
                                break;
                            }
                            case Controller::Type::MOUSE:
                            {
                                connectEvent.type = Event::Type::MOUSE_CONNECT;
                                std::unique_ptr<Mouse> mouse(new Mouse(event.deviceId));
                                connectEvent.mouseEvent.mouse = mouse.get();
                                controllers.insert(std::make_pair(event.deviceId, std::move(mouse)));
                                break;
                            }
                            case Controller::Type::TOUCHPAD:
                            {
                                connectEvent.type = Event::Type::TOUCHPAD_CONNECT;
                                std::unique_ptr<Touchpad> touchpad(new Touchpad(event.deviceId));
                                connectEvent.touchEvent.touchpad = touchpad.get();
                                controllers.insert(std::make_pair(event.deviceId, std::move(touchpad)));
                                break;
                            }
                        }

                        engine->getEventDispatcher().postEvent(connectEvent, true);
                        break;
                    }
                    case InputSystem::Event::Type::DEVICE_DISCONNECT:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
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
                                    break;
                                }
                                case Controller::Type::MOUSE:
                                {
                                    disconnectEvent.type = Event::Type::MOUSE_DISCONNECT;
                                    disconnectEvent.mouseEvent.mouse = static_cast<Mouse*>(i->second.get());
                                    break;
                                }
                                case Controller::Type::TOUCHPAD:
                                {
                                    disconnectEvent.type = Event::Type::TOUCHPAD_DISCONNECT;
                                    disconnectEvent.touchEvent.touchpad = static_cast<Touchpad*>(i->second.get());
                                    break;
                                }
                            }

                            engine->getEventDispatcher().postEvent(disconnectEvent, true);
                            controllers.erase(i);
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
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Gamepad* gamepad = static_cast<Gamepad*>(i->second.get());
                            gamepad->handleButtonValueChange(event.gamepadButton, event.pressed, event.value);
                        }

                        break;
                    }
                    case InputSystem::Event::Type::KEY_PRESS:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Keyboard* keyboard = static_cast<Keyboard*>(i->second.get());
                            keyboard->handleKeyPress(event.keyboardKey, event.modifiers);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::KEY_RELEASE:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Keyboard* keyboard = static_cast<Keyboard*>(i->second.get());
                            keyboard->handleKeyRelease(event.keyboardKey, event.modifiers);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_PRESS:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Mouse* mouse = static_cast<Mouse*>(i->second.get());
                            mouse->handleButtonPress(event.mouseButton, event.position, event.modifiers);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_RELEASE:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Mouse* mouse = static_cast<Mouse*>(i->second.get());
                            mouse->handleButtonRelease(event.mouseButton, event.position, event.modifiers);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_SCROLL:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Mouse* mouse = static_cast<Mouse*>(i->second.get());
                            mouse->handleScroll(event.scroll, event.position, event.modifiers);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::MOUSE_MOVE:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Mouse* mouse = static_cast<Mouse*>(i->second.get());
                            mouse->handleMove(event.position, event.modifiers);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::TOUCH_BEGIN:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Touchpad* touchpad = static_cast<Touchpad*>(i->second.get());
                            touchpad->handleTouchBegin(event.touchId, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::TOUCH_MOVE:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Touchpad* touchpad = static_cast<Touchpad*>(i->second.get());
                            touchpad->handleTouchMove(event.touchId, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::TOUCH_END:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Touchpad* touchpad = static_cast<Touchpad*>(i->second.get());
                            touchpad->handleTouchEnd(event.touchId, event.position);
                        }
                        break;
                    }
                    case InputSystem::Event::Type::TOUCH_CANCEL:
                    {
                        auto i = controllers.find(event.deviceId);
                        if (i != controllers.end())
                        {
                            Touchpad* touchpad = static_cast<Touchpad*>(i->second.get());
                            touchpad->handleTouchCancel(event.touchId, event.position);
                        }
                        break;
                    }
                }
            }
        }

        void InputManager::setCurrentCursor(Cursor* cursor)
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            if (cursor)
                currentNativeCursor = cursor->getNativeCursor();
            else
                currentNativeCursor = nullptr;

            engine->executeOnMainThread(std::bind(&InputManager::activateNativeCursor,
                                                  this,
                                                  currentNativeCursor));
        }

        void InputManager::activateNativeCursor(NativeCursor*)
        {
        }

        NativeCursor* InputManager::createNativeCursor()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            std::unique_ptr<NativeCursor> cursorResource(new NativeCursor(*this));
            NativeCursor* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputManager::deleteNativeCursor(NativeCursor* resource)
        {
            engine->executeOnMainThread([this, resource] {
                std::unique_lock<std::mutex> lock(resourceMutex);

                auto i = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<NativeCursor>& ptr) {
                    return ptr.get() == resource;
                });

                if (i != resources.end())
                {
                    resourceDeleteSet.push_back(std::move(*i));
                    resources.erase(i);
                }

                if (resource == currentNativeCursor)
                {
                    // remove the cursor
                    currentNativeCursor = nullptr;
                    activateNativeCursor(nullptr);
                }

                resourceDeleteSet.clear();
            });
        }

        void InputManager::setCursorVisible(bool)
        {
        }

        bool InputManager::isCursorVisible() const
        {
            return false;
        }

        void InputManager::setCursorLocked(bool)
        {
        }

        bool InputManager::isCursorLocked() const
        {
            return false;
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
