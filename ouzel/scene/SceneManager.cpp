// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SceneManager.h"
#include "Scene.h"
#include "core/Engine.h"
#include "Node.h"

namespace ouzel
{
    namespace scene
    {
        SceneManager::SceneManager()
        {
            sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);
            eventHandler.windowHandler = std::bind(&SceneManager::handleWindow, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.mouseHandler = std::bind(&SceneManager::handleMouse, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.touchHandler = std::bind(&SceneManager::handleTouch, this, std::placeholders::_1, std::placeholders::_2);
        }

        SceneManager::~SceneManager()
        {
        }

        void SceneManager::setScene(const ScenePtr& newScene)
        {
            if (scene != newScene)
            {
                scene = newScene;

                if (scene)
                {
                    scene->recalculateProjection();
                }
            }
        }

        void SceneManager::draw()
        {
            if (scene)
            {
                scene->draw();
            }
        }

        void SceneManager::recalculateProjection()
        {
            if (scene)
            {
                scene->recalculateProjection();
            }
        }

        bool SceneManager::handleWindow(Event::Type type, const WindowEvent&)
        {
            if (type == Event::Type::WINDOW_SIZE_CHANGE ||
                type == Event::Type::WINDOW_FULLSCREEN_CHANGE)
            {
                recalculateProjection();
            }

            return true;
        }

        bool SceneManager::handleMouse(Event::Type type, const MouseEvent& event)
        {
            if (scene)
            {
                switch (type)
                {
                    case Event::Type::MOUSE_DOWN:
                    {
                        scene::NodePtr node = scene->pickNode(event.position);
                        pointerDownOnNode(0, node, event.position);
                        break;
                    }
                    case Event::Type::MOUSE_UP:
                    {
                        scene::NodePtr node = scene->pickNode(event.position);
                        pointerUpOnNode(0, node, event.position);
                        break;
                    }
                    case Event::Type::MOUSE_MOVE:
                    {
                        scene::NodePtr node = scene->pickNode(event.position);
                        pointerEnterNode(0, node, event.position);

                        if (scene::NodePtr pointerDownOnNode = getPointerDownOnNode(0))
                        {
                            pointerDragNode(0, pointerDownOnNode, event.position);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            return true;
        }

        bool SceneManager::handleTouch(Event::Type type, const TouchEvent& event)
        {
            if (scene)
            {
                switch (type)
                {
                    case Event::Type::TOUCH_BEGIN:
                    {
                        scene::NodePtr node = scene->pickNode(event.position);
                        pointerDownOnNode(event.touchId, node, event.position);
                        break;
                    }
                    case Event::Type::TOUCH_END:
                    {
                        scene::NodePtr node = scene->pickNode(event.position);
                        pointerUpOnNode(event.touchId, node, event.position);
                        break;
                    }
                    case Event::Type::TOUCH_MOVE:
                    {
                        scene::NodePtr node = scene->pickNode(event.position);
                        pointerEnterNode(0, node, event.position);
                        
                        if (scene::NodePtr pointerDownOnNode = getPointerDownOnNode(event.touchId))
                        {
                            pointerDragNode(event.touchId, pointerDownOnNode, event.position);
                        }
                        break;
                    }
                    case Event::Type::TOUCH_CANCEL:
                    {
                        scene::NodePtr node = scene->pickNode(event.position);
                        pointerUpOnNode(event.touchId, node, event.position);
                        break;
                    }
                    default:
                        break;
                }
            }

            return true;
        }

        scene::NodePtr SceneManager::getPointerOnNode(uint64_t pointerId) const
        {
            scene::NodePtr result;

            auto i = pointerOnNodes.find(pointerId);

            if (i != pointerOnNodes.end() && !i->second.expired())
            {
                result = i->second.lock();
            }

            return result;
        }

        scene::NodePtr SceneManager::getPointerDownOnNode(uint64_t pointerId) const
        {
            scene::NodePtr result;

            auto i = pointerDownOnNodes.find(pointerId);

            if (i != pointerDownOnNodes.end() && !i->second.expired())
            {
                result = i->second.lock();
            }

            return result;
        }

        void SceneManager::pointerEnterNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
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
                event.type = Event::Type::UI_ENTER_NODE;

                event.uiEvent.node = node;
                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

        void SceneManager::pointerLeaveNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
        {
            if (node && node->isReceivingInput())
            {
                Event event;
                event.type = Event::Type::UI_LEAVE_NODE;

                event.uiEvent.node = node;
                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }

            pointerOnNodes.erase(pointerId);
        }

        void SceneManager::pointerDownOnNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
        {
            pointerDownOnNodes[pointerId] = node;

            if (node && node->isReceivingInput())
            {
                Event event;
                event.type = Event::Type::UI_PRESS_NODE;

                event.uiEvent.node = node;
                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }

        void SceneManager::pointerUpOnNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
        {
            scene::NodePtr pointerDownOnNode = getPointerDownOnNode(pointerId);

            if (pointerDownOnNode && pointerDownOnNode->isReceivingInput())
            {
                Event releaseEvent;
                releaseEvent.type = Event::Type::UI_RELEASE_NODE;

                releaseEvent.uiEvent.node = pointerDownOnNode;
                releaseEvent.uiEvent.position = pointerDownOnNode->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(releaseEvent);

                if (pointerDownOnNode == node)
                {
                    Event clickEvent;
                    clickEvent.type = Event::Type::UI_CLICK_NODE;

                    clickEvent.uiEvent.node = pointerDownOnNode;
                    clickEvent.uiEvent.position = pointerDownOnNode->convertWorldToLocal(position);

                    sharedEngine->getEventDispatcher()->dispatchEvent(clickEvent);
                }
            }

            pointerDownOnNodes.erase(pointerId);
        }

        void SceneManager::pointerDragNode(uint64_t, const scene::NodePtr& node, const Vector2& position)
        {
            if (node && node->isReceivingInput())
            {
                Event event;
                event.type = Event::Type::UI_DRAG_NODE;

                event.uiEvent.node = node;
                event.uiEvent.position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(event);
            }
        }
    } // namespace scene
} // namespace ouzel
