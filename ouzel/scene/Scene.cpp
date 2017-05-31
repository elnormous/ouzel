// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"
#include "SceneManager.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"

namespace ouzel
{
    namespace scene
    {
        Scene::Scene():
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.windowHandler = std::bind(&Scene::handleWindow, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.mouseHandler = std::bind(&Scene::handleMouse, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.touchHandler = std::bind(&Scene::handleTouch, this, std::placeholders::_1, std::placeholders::_2);
        }

        Scene::~Scene()
        {
            if (sceneManger) sceneManger->removeScene(this);

            for (Layer* layer : layers)
            {
                if (entered) layer->leave();
                layer->scene = nullptr;
            }
        }

        void Scene::draw()
        {
            std::stable_sort(layers.begin(), layers.end(), [](Layer* a, Layer* b) {
                return a->getOrder() > b->getOrder();
            });

            for (Layer* layer : layers)
            {
                layer->draw();
            }
        }

        void Scene::addChildLayer(Layer* layer)
        {
            if (layer && !hasLayer(layer))
            {
                layers.push_back(layer);

                if (entered) layer->enter();

                layer->scene = this;
            }
        }

        bool Scene::removeChildLayer(Layer* layer)
        {
            bool result = false;

            std::vector<Layer*>::iterator layerIterator = std::find(layers.begin(), layers.end(), layer);

            if (layerIterator != layers.end())
            {
                if (entered)
                {
                    layer->leave();
                }

                layers.erase(layerIterator);

                layer->scene = nullptr;

                result = true;
            }

            std::vector<std::unique_ptr<Layer>>::iterator ownedIterator = std::find_if(ownedLayers.begin(), ownedLayers.end(), [layer](const std::unique_ptr<Layer>& other) {
                return other.get() == layer;
            });

            if (ownedIterator != ownedLayers.end())
            {
                ownedLayers.erase(ownedIterator);
            }

            return result;
        }

        void Scene::removeAllLayers()
        {
            if (entered)
            {
                for (Layer* layer : layers)
                {
                    layer->leave();
                }
            }

            layers.clear();
            ownedLayers.clear();
        }

        bool Scene::hasLayer(Layer* layer) const
        {
            std::vector<Layer*>::const_iterator i = std::find(layers.begin(), layers.end(), layer);

            return i != layers.end();
        }

        void Scene::recalculateProjection()
        {
            for (Layer* layer : layers)
            {
                layer->recalculateProjection();
            }
        }

        Node* Scene::pickNode(const Vector2& position, bool renderTargets) const
        {
            for (std::vector<Layer*>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                Layer* layer = *i;

                if (Node* result = layer->pickNode(position, renderTargets))
                {
                    return result;
                }
            }

            return nullptr;
        }

        std::vector<Node*> Scene::pickNodes(const Vector2& position, bool renderTargets) const
        {
            std::vector<Node*> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<Node*> nodes = (*i)->pickNodes(position, renderTargets);

                result.insert(result.end(), nodes.begin(), nodes.end());
            }

            return result;
        }

        std::vector<Node*> Scene::pickNodes(const std::vector<Vector2>& edges, bool renderTargets) const
        {
            std::vector<Node*> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<Node*> nodes = (*i)->pickNodes(edges, renderTargets);

                result.insert(result.end(), nodes.begin(), nodes.end());
            }

            return result;
        }

        void Scene::enter()
        {
            entered = true;

            recalculateProjection();
            sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

            for (Layer* layer : layers)
            {
                layer->enter();
            }
        }

        void Scene::leave()
        {
            entered = false;

            eventHandler.remove();

            for (Layer* layer : layers)
            {
                layer->leave();
            }
        }

        bool Scene::handleWindow(Event::Type type, const WindowEvent&)
        {
            if (type == Event::Type::WINDOW_SIZE_CHANGE ||
                type == Event::Type::WINDOW_CONTENT_SCALE_CHANGE)
            {
                recalculateProjection();
            }

            return true;
        }

        bool Scene::handleMouse(Event::Type type, const MouseEvent& event)
        {
            switch (type)
            {
                case Event::Type::MOUSE_DOWN:
                {
                    Node* node = pickNode(event.position);
                    pointerDownOnNode(0, node, event.position);
                    break;
                }
                case Event::Type::MOUSE_UP:
                {
                    Node* node = pickNode(event.position);
                    pointerUpOnNode(0, node, event.position);
                    break;
                }
                case Event::Type::MOUSE_MOVE:
                {
                    Node* previousNode = pickNode(event.position - event.difference);
                    pointerLeaveNode(0, previousNode, event.position);

                    Node* node = pickNode(event.position);
                    pointerEnterNode(0, node, event.position);

                    auto i = pointerDownOnNodes.find(0);

                    if (i != pointerDownOnNodes.end())
                    {
                        pointerDragNode(0, i->second, event.position, event.difference);
                    }
                    break;
                }
                default:
                    break;
            }

            return true;
        }

        bool Scene::handleTouch(Event::Type type, const TouchEvent& event)
        {
            switch (type)
            {
                case Event::Type::TOUCH_BEGIN:
                {
                    Node* node = pickNode(event.position);
                    pointerDownOnNode(event.touchId, node, event.position);
                    break;
                }
                case Event::Type::TOUCH_END:
                {
                    Node* node = pickNode(event.position);
                    pointerUpOnNode(event.touchId, node, event.position);
                    break;
                }
                case Event::Type::TOUCH_MOVE:
                {
                    Node* previousNode = pickNode(event.position - event.difference);
                    pointerLeaveNode(0, previousNode, event.position);

                    Node* node = pickNode(event.position);
                    pointerEnterNode(0, node, event.position);

                    auto i = pointerDownOnNodes.find(event.touchId);

                    if (i != pointerDownOnNodes.end())
                    {
                        pointerDragNode(event.touchId, i->second, event.position, event.difference);
                    }
                    break;
                }
                case Event::Type::TOUCH_CANCEL:
                {
                    Node* node = pickNode(event.position);
                    pointerUpOnNode(event.touchId, node, event.position);
                    break;
                }
                default:
                    break;
            }

            return true;
        }

        void Scene::pointerEnterNode(uint64_t pointerId, Node* node, const Vector2& position)
        {
            if (node)
            {
                Event event;
                event.type = Event::Type::UI_ENTER_NODE;

                event.uiEvent.node = node;
                event.uiEvent.touchId = pointerId;
                event.uiEvent.position = position;

                sharedEngine->getEventDispatcher()->postEvent(event);
            }
        }

        void Scene::pointerLeaveNode(uint64_t pointerId, Node* node, const Vector2& position)
        {
            if (node)
            {
                Event event;
                event.type = Event::Type::UI_LEAVE_NODE;

                event.uiEvent.node = node;
                event.uiEvent.touchId = pointerId;
                event.uiEvent.position = position;

                sharedEngine->getEventDispatcher()->postEvent(event);
            }
        }

        void Scene::pointerDownOnNode(uint64_t pointerId, Node* node, const Vector2& position)
        {
            pointerDownOnNodes[pointerId] = node;

            if (node)
            {
                Event event;
                event.type = Event::Type::UI_PRESS_NODE;

                event.uiEvent.node = node;
                event.uiEvent.touchId = pointerId;
                event.uiEvent.position = position;

                sharedEngine->getEventDispatcher()->postEvent(event);
            }
        }

        void Scene::pointerUpOnNode(uint64_t pointerId, Node* node, const Vector2& position)
        {
            auto i = pointerDownOnNodes.find(pointerId);

            if (i != pointerDownOnNodes.end())
            {
                auto pointerDownOnNode = i->second;

                if (pointerDownOnNode)
                {
                    Event releaseEvent;
                    releaseEvent.type = Event::Type::UI_RELEASE_NODE;

                    releaseEvent.uiEvent.node = pointerDownOnNode;
                    releaseEvent.uiEvent.touchId = pointerId;
                    releaseEvent.uiEvent.position = position;

                    sharedEngine->getEventDispatcher()->postEvent(releaseEvent);

                    if (pointerDownOnNode == node)
                    {
                        Event clickEvent;
                        clickEvent.type = Event::Type::UI_CLICK_NODE;

                        clickEvent.uiEvent.node = pointerDownOnNode;
                        clickEvent.uiEvent.touchId = pointerId;
                        clickEvent.uiEvent.position = position;

                        sharedEngine->getEventDispatcher()->postEvent(clickEvent);
                    }
                }
            }

            pointerDownOnNodes.erase(pointerId);
        }

        void Scene::pointerDragNode(uint64_t pointerId, Node* node, const Vector2& position, const Vector2& difference)
        {
            if (node)
            {
                Event event;
                event.type = Event::Type::UI_DRAG_NODE;

                event.uiEvent.node = node;
                event.uiEvent.touchId = pointerId;
                event.uiEvent.difference = difference;
                event.uiEvent.position = position;

                sharedEngine->getEventDispatcher()->postEvent(event);
            }
        }
    } // namespace scene
} // namespace ouzel
