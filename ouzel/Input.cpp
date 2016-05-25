// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "Input.h"
#include "Engine.h"
#include "EventDispatcher.h"
#include "Scene.h"
#include "Node.h"
#include "Utils.h"

namespace ouzel
{
    namespace input
    {
        Input::Input()
        {
            memset(keyboardKeyStates, 0, sizeof(keyboardKeyStates));
            memset(mouseButtonStates, 0, sizeof(mouseButtonStates));
        }

        Input::~Input()
        {

        }

        void Input::update()
        {

        }

        void Input::setCursorVisible(bool visible)
        {
            OUZEL_UNUSED(visible);
        }

        bool Input::isCursorVisible() const
        {
            return false;
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
            event.sender = shared_from_this();

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            if (!keyboardKeyStates[static_cast<uint32_t>(key)])
            {
                keyboardKeyStates[static_cast<uint32_t>(key)] = true;

                event.type = Event::Type::KEY_DOWN;
                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
            else
            {
                event.type = Event::Type::KEY_REPEAT;
                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

        void Input::keyUp(KeyboardKey key, uint32_t modifiers)
        {
            keyboardKeyStates[static_cast<uint32_t>(key)] = false;

            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::KEY_UP;

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }

        void Input::mouseDown(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = true;

            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::MOUSE_DOWN;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);

            if (scene::ScenePtr scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::NodePtr node = scene->pickNode(position);
                mouseDownOnNode(node, position);
            }
        }

        void Input::mouseUp(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = false;

            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::MOUSE_UP;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);

            if (scene::ScenePtr scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::NodePtr node = scene->pickNode(position);
                mouseUpOnNode(node, position);
            }
        }

        void Input::mouseMove(const Vector2& position, uint32_t modifiers)
        {
            cursorPosition = position;

            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::MOUSE_MOVE;

            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);

            if (scene::ScenePtr scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::NodePtr node = scene->pickNode(position);
                mouseEnterNode(node, position);
            }

            if (!mouseDownNode.expired())
            {
                mouseDragNode(mouseDownNode.lock(), position);
            }
        }

        void Input::mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::MOUSE_SCROLL;

            event.mouseEvent.position = position;
            event.mouseEvent.scroll = scroll;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }

        void Input::touchBegin(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::TOUCH_BEGIN;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }

        void Input::touchEnd(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::TOUCH_END;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }

        void Input::touchMove(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::TOUCH_MOVE;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }

        void Input::touchCancel(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::TOUCH_CANCEL;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }

        void Input::mouseEnterNode(const scene::NodePtr& node, const Vector2& position)
        {
            scene::NodePtr mouseOnNode = mouseNode.lock();

            if (mouseOnNode)
            {
                if (mouseOnNode == node)
                {
                    return;
                }
                else
                {
                    mouseLeaveNode(mouseOnNode, position);
                }
            }

            mouseNode = node;

            if (node && node->isReceivingInput())
            {
                Event event;
                event.sender = node;
                event.type = Event::Type::UI_ENTER_NODE;

                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

        void Input::mouseLeaveNode(const scene::NodePtr& node, const Vector2& position)
        {
            if (node && node->isReceivingInput())
            {
                Event event;
                event.sender = node;
                event.type = Event::Type::UI_LEAVE_NODE;

                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

        void Input::mouseDownOnNode(const scene::NodePtr& node, const Vector2& position)
        {
            mouseDownNode = node;

            if (node && node->isReceivingInput())
            {
                Event event;
                event.sender = node;
                event.type = Event::Type::UI_PRESS_NODE;

                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

        void Input::mouseUpOnNode(const scene::NodePtr& node, const Vector2& position)
        {
            scene::NodePtr mouseDownOnNode = mouseDownNode.lock();

            if (mouseDownOnNode && mouseDownOnNode->isReceivingInput())
            {
                Event releaseEvent;
                releaseEvent.sender = mouseDownOnNode;
                releaseEvent.type = Event::Type::UI_RELEASE_NODE;

                releaseEvent.uiEvent.position = mouseDownOnNode->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(releaseEvent);

                if (mouseDownOnNode == node)
                {
                    Event clickEvent;
                    clickEvent.sender = mouseDownOnNode;
                    clickEvent.type = Event::Type::UI_CLICK_NODE;

                    clickEvent.uiEvent.position = mouseDownOnNode->convertWorldToLocal(position);

                    sharedEngine->getEventDispatcher()->dispatchEvent(clickEvent);
                }
            }

            mouseDownNode.reset();
        }

        void Input::mouseDragNode(const scene::NodePtr& node, const Vector2& position)
        {
            Event event;
            event.sender = node;
            event.type = Event::Type::UI_DRAG_NODE;

            event.uiEvent.position = node->convertWorldToLocal(position);

            sharedEngine->getEventDispatcher()->dispatchEvent(event);
        }

    } // namespace input
} // namespace ouzel
