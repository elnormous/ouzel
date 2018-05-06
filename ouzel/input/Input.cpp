// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Input.hpp"
#include "CursorResource.hpp"
#include "Gamepad.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "math/MathUtils.hpp"
#include "thread/Lock.hpp"

namespace ouzel
{
    namespace input
    {
        Input::Input()
        {
            std::fill(std::begin(keyboardKeyStates), std::end(keyboardKeyStates), false);
            std::fill(std::begin(mouseButtonStates), std::end(mouseButtonStates), false);
        }

        Input::~Input()
        {
        }

        bool Input::init()
        {
            return true;
        }

        void Input::setCurrentCursor(Cursor* cursor)
        {
            Lock lock(resourceMutex);

            if (cursor)
            {
                currentCursorResource = cursor->getResource();
            }
            else
            {
                currentCursorResource = nullptr;
            }

            engine->executeOnMainThread(std::bind(&Input::activateCursorResource,
                                                        this,
                                                        currentCursorResource));
        }

        void Input::activateCursorResource(CursorResource*)
        {
        }

        CursorResource* Input::createCursorResource()
        {
            Lock lock(resourceMutex);

            std::unique_ptr<CursorResource> cursorResource(new CursorResource(*this));
            CursorResource* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void Input::deleteCursorResource(CursorResource* resource)
        {
            engine->executeOnMainThread([this, resource] {
                Lock lock(resourceMutex);

                std::vector<std::unique_ptr<CursorResource>>::iterator i = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<CursorResource>& ptr) {
                    return ptr.get() == resource;
                });

                if (i != resources.end())
                {
                    resourceDeleteSet.push_back(std::move(*i));
                    resources.erase(i);
                }

                if (resource == currentCursorResource)
                {
                    // remove the cursor
                    currentCursorResource = nullptr;
                    activateCursorResource(nullptr);
                }

                resourceDeleteSet.clear();
            });
        }

        void Input::setCursorVisible(bool)
        {
        }

        bool Input::isCursorVisible() const
        {
            return false;
        }

        void Input::setCursorLocked(bool)
        {
        }

        bool Input::isCursorLocked() const
        {
            return false;
        }

        void Input::setCursorPosition(const Vector2& position)
        {
            cursorPosition = position;
        }

        void Input::startGamepadDiscovery()
        {
        }

        void Input::stopGamepadDiscovery()
        {
        }

        void Input::keyPress(KeyboardKey key, uint32_t modifiers)
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

        void Input::keyRelease(KeyboardKey key, uint32_t modifiers)
        {
            keyboardKeyStates[static_cast<uint32_t>(key)] = false;

            Event event;
            event.type = Event::Type::KEY_RELEASE;

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            engine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseButtonPress(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = true;

            Event event;
            event.type = Event::Type::MOUSE_PRESS;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseButtonRelease(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = false;

            Event event;
            event.type = Event::Type::MOUSE_RELEASE;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseMove(const Vector2& position, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_MOVE;

            event.mouseEvent.difference = position - cursorPosition;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            cursorPosition = position;

            engine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseRelativeMove(const Vector2& relativePosition, uint32_t modifiers)
        {
            Vector2 newPosition = cursorPosition + relativePosition;

            newPosition.x = clamp(newPosition.x, 0.0F, 1.0F);
            newPosition.y = clamp(newPosition.y, 0.0F, 1.0F);

            mouseMove(newPosition, modifiers);
        }

        void Input::mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_SCROLL;

            event.mouseEvent.position = position;
            event.mouseEvent.scroll = scroll;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchBegin(uint64_t touchId, const Vector2& position, float force)
        {
            Event event;
            event.type = Event::Type::TOUCH_BEGIN;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;
            event.touchEvent.force = force;

            touchPositions[touchId] = position;

            engine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchEnd(uint64_t touchId, const Vector2& position, float force)
        {
            Event event;
            event.type = Event::Type::TOUCH_END;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;
            event.touchEvent.force = force;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
            {
                touchPositions.erase(i);
            }

            engine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchMove(uint64_t touchId, const Vector2& position, float force)
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

        void Input::touchCancel(uint64_t touchId, const Vector2& position, float force)
        {
            Event event;
            event.type = Event::Type::TOUCH_CANCEL;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;
            event.touchEvent.force = force;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
            {
                touchPositions.erase(i);
            }

            engine->getEventDispatcher()->postEvent(event);
        }

        bool Input::showVirtualKeyboard()
        {
            return false;
        }

        bool Input::hideVirtualKeyboard()
        {
            return false;
        }
    } // namespace input
} // namespace ouzel
