// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Input.h"
#include "CursorResource.h"
#include "Gamepad.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"
#include "math/MathUtils.h"

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
            std::lock_guard<std::mutex> lock(resourceMutex);

            if (cursor)
            {
                currentCursorResource = cursor->getResource();
            }
            else
            {
                currentCursorResource = nullptr;
            }

            CursorResource* resource = currentCursorResource;

            sharedApplication->execute([this, resource] {
                activateCursorResource(resource);
            });
        }

        void Input::activateCursorResource(CursorResource*)
        {
        }

        CursorResource* Input::createCursorResource()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            std::unique_ptr<CursorResource> cursorResource(new CursorResource());
            CursorResource* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void Input::deleteCursorResource(CursorResource* resource)
        {
            sharedApplication->execute([this, resource] {
                std::lock_guard<std::mutex> lock(resourceMutex);

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

                    sharedApplication->execute([this] {
                        activateCursorResource(nullptr);
                    });
                }

                resourceDeleteSet.clear();
            });
        }
        
        void Input::uploadCursorResource(CursorResource* resource)
        {
            sharedApplication->execute([this, resource] {
                resource->upload();
                if (resource == currentCursorResource) activateCursorResource(currentCursorResource);
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

        void Input::keyDown(KeyboardKey key, uint32_t modifiers)
        {
            Event event;

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            if (!keyboardKeyStates[static_cast<uint32_t>(key)])
            {
                keyboardKeyStates[static_cast<uint32_t>(key)] = true;

                event.type = Event::Type::KEY_DOWN;
                sharedEngine->getEventDispatcher()->postEvent(event);
            }
            else
            {
                event.type = Event::Type::KEY_REPEAT;
                sharedEngine->getEventDispatcher()->postEvent(event);
            }
        }

        void Input::keyUp(KeyboardKey key, uint32_t modifiers)
        {
            keyboardKeyStates[static_cast<uint32_t>(key)] = false;

            Event event;
            event.type = Event::Type::KEY_UP;

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseDown(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = true;

            Event event;
            event.type = Event::Type::MOUSE_DOWN;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseUp(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = false;

            Event event;
            event.type = Event::Type::MOUSE_UP;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseMove(const Vector2& position, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_MOVE;

            event.mouseEvent.difference = position - cursorPosition;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            cursorPosition = position;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseRelativeMove(const Vector2& relativePosition, uint32_t modifiers)
        {
            Vector2 newPosition = cursorPosition + relativePosition;

            newPosition.v[0] = clamp(newPosition.v[0], 0.0f, 1.0f);
            newPosition.v[1] = clamp(newPosition.v[1], 0.0f, 1.0f);

            mouseMove(newPosition, modifiers);
        }

        void Input::mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_SCROLL;

            event.mouseEvent.position = position;
            event.mouseEvent.scroll = scroll;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchBegin(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.type = Event::Type::TOUCH_BEGIN;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            touchPositions[touchId] = position;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchEnd(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.type = Event::Type::TOUCH_END;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
            {
                touchPositions.erase(i);
            }

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchMove(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.type = Event::Type::TOUCH_MOVE;

            event.touchEvent.touchId = touchId;
            event.touchEvent.difference = position - touchPositions[touchId];
            event.touchEvent.position = position;

            touchPositions[touchId] = position;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchCancel(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.type = Event::Type::TOUCH_CANCEL;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
            {
                touchPositions.erase(i);
            }

            sharedEngine->getEventDispatcher()->postEvent(event);
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
