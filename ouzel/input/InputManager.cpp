// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "InputManager.hpp"
#include "NativeCursor.hpp"
#include "Gamepad.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace input
    {
        InputManager::InputManager()
        {
            std::fill(std::begin(keyboardKeyStates), std::end(keyboardKeyStates), false);
            std::fill(std::begin(mouseButtonStates), std::end(mouseButtonStates), false);
        }

        InputManager::~InputManager()
        {
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

        void InputManager::setCursorPosition(const Vector2& position)
        {
            cursorPosition = position;
        }

        void InputManager::startDeviceDiscovery()
        {
        }

        void InputManager::stopDeviceDiscovery()
        {
        }

        void InputManager::keyPress(KeyboardKey key, uint32_t modifiers)
        {
            Event event;

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            if (!keyboardKeyStates[static_cast<uint32_t>(key)])
            {
                keyboardKeyStates[static_cast<uint32_t>(key)] = true;

                event.type = Event::Type::KEY_PRESS;
                engine->getEventDispatcher()->postEvent(event);
            }
            else
            {
                event.type = Event::Type::KEY_REPEAT;
                engine->getEventDispatcher()->postEvent(event);
            }
        }

        void InputManager::keyRelease(KeyboardKey key, uint32_t modifiers)
        {
            keyboardKeyStates[static_cast<uint32_t>(key)] = false;

            Event event;
            event.type = Event::Type::KEY_RELEASE;

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::mouseButtonPress(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = true;

            Event event;
            event.type = Event::Type::MOUSE_PRESS;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::mouseButtonRelease(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = false;

            Event event;
            event.type = Event::Type::MOUSE_RELEASE;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::mouseMove(const Vector2& position, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_MOVE;

            event.mouseEvent.difference = position - cursorPosition;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            cursorPosition = position;

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::mouseRelativeMove(const Vector2& relativePosition, uint32_t modifiers)
        {
            Vector2 newPosition = cursorPosition + relativePosition;

            newPosition.x = clamp(newPosition.x, 0.0F, 1.0F);
            newPosition.y = clamp(newPosition.y, 0.0F, 1.0F);

            mouseMove(newPosition, modifiers);
        }

        void InputManager::mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_SCROLL;

            event.mouseEvent.position = position;
            event.mouseEvent.scroll = scroll;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::touchBegin(uint64_t touchId, const Vector2& position, float force)
        {
            Event event;
            event.type = Event::Type::TOUCH_BEGIN;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;
            event.touchEvent.force = force;

            touchPositions[touchId] = position;

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::touchEnd(uint64_t touchId, const Vector2& position, float force)
        {
            Event event;
            event.type = Event::Type::TOUCH_END;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;
            event.touchEvent.force = force;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
                touchPositions.erase(i);

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::touchMove(uint64_t touchId, const Vector2& position, float force)
        {
            Event event;
            event.type = Event::Type::TOUCH_MOVE;

            event.touchEvent.touchId = touchId;
            event.touchEvent.difference = position - touchPositions[touchId];
            event.touchEvent.position = position;
            event.touchEvent.force = force;

            touchPositions[touchId] = position;

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::touchCancel(uint64_t touchId, const Vector2& position, float force)
        {
            Event event;
            event.type = Event::Type::TOUCH_CANCEL;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;
            event.touchEvent.force = force;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
                touchPositions.erase(i);

            engine->getEventDispatcher()->postEvent(event);
        }

        void InputManager::showVirtualKeyboard()
        {
        }

        void InputManager::hideVirtualKeyboard()
        {
        }
    } // namespace input
} // namespace ouzel
