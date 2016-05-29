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
                pointerDownOnNode(0, node, position);
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
                pointerUpOnNode(0, node, position);
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
                pointerEnterNode(0, node, position);
            }

            if (scene::NodePtr pointerDownOnNode = getPointerDownOnNode(0))
            {
                pointerDragNode(0, pointerDownOnNode, position);
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

            if (scene::ScenePtr scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::NodePtr node = scene->pickNode(position);
                pointerDownOnNode(touchId, node, position);
            }
        }

        void Input::touchEnd(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::TOUCH_END;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);

            if (scene::ScenePtr scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::NodePtr node = scene->pickNode(position);
                pointerUpOnNode(touchId, node, position);
            }
        }

        void Input::touchMove(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::TOUCH_MOVE;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);

            if (scene::NodePtr pointerDownOnNode = getPointerDownOnNode(touchId))
            {
                pointerDragNode(touchId, pointerDownOnNode, position);
            }
        }

        void Input::touchCancel(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::TOUCH_CANCEL;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);

            if (scene::ScenePtr scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::NodePtr node = scene->pickNode(position);
                pointerUpOnNode(touchId, node, position);
            }
        }

        scene::NodePtr Input::getPointerOnNode(uint64_t pointerId) const
        {
            scene::NodePtr result;

            auto i = pointerOnNodes.find(pointerId);

            if (i != pointerOnNodes.end() && !i->second.expired())
            {
                result = i->second.lock();
            }

            return result;
        }

        scene::NodePtr Input::getPointerDownOnNode(uint64_t pointerId) const
        {
            scene::NodePtr result;

            auto i = pointerDownOnNodes.find(pointerId);

            if (i != pointerDownOnNodes.end() && !i->second.expired())
            {
                result = i->second.lock();
            }

            return result;
        }

        void Input::pointerEnterNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
        {
            scene::NodePtr pointerOnNode = getPointerOnNode(pointerId);

            if (pointerOnNode)
            {
                if (pointerOnNode == node)
                {
                    return;
                }
                else
                {
                    pointerLeaveNode(pointerId, pointerOnNode, position);
                }
            }

            pointerOnNodes[pointerId] = node;

            if (node && node->isReceivingInput())
            {
                Event event;
                event.sender = node;
                event.type = Event::Type::UI_ENTER_NODE;

                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

        void Input::pointerLeaveNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
        {
            if (node && node->isReceivingInput())
            {
                Event event;
                event.sender = node;
                event.type = Event::Type::UI_LEAVE_NODE;

                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }

            pointerOnNodes.erase(pointerId);
        }

        void Input::pointerDownOnNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
        {
            pointerDownOnNodes[pointerId] = node;

            if (node && node->isReceivingInput())
            {
                Event event;
                event.sender = node;
                event.type = Event::Type::UI_PRESS_NODE;

                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

        void Input::pointerUpOnNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
        {
            scene::NodePtr pointerDownOnNode = getPointerDownOnNode(pointerId);

            if (pointerDownOnNode && pointerDownOnNode->isReceivingInput())
            {
                Event releaseEvent;
                releaseEvent.sender = pointerDownOnNode;
                releaseEvent.type = Event::Type::UI_RELEASE_NODE;

                releaseEvent.uiEvent.position = pointerDownOnNode->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(releaseEvent);

                if (pointerDownOnNode == node)
                {
                    Event clickEvent;
                    clickEvent.sender = pointerDownOnNode;
                    clickEvent.type = Event::Type::UI_CLICK_NODE;

                    clickEvent.uiEvent.position = pointerDownOnNode->convertWorldToLocal(position);

                    sharedEngine->getEventDispatcher()->dispatchEvent(clickEvent);
                }
            }

            pointerDownOnNodes.erase(pointerId);
        }

        void Input::pointerDragNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
        {
            if (node && node->isReceivingInput())
            {
                Event event;
                event.sender = node;
                event.type = Event::Type::UI_DRAG_NODE;

                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

    } // namespace input
} // namespace ouzel
