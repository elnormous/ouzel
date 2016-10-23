// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"
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
            if (entered)
            {
                for (const LayerPtr& layer : layers)
                {
                    layer->leave();
                }
            }
        }

        void Scene::draw()
        {
            std::stable_sort(layers.begin(), layers.end(), [](LayerPtr a, LayerPtr b) {
                return a->getOrder() > b->getOrder();
            });

            for (const LayerPtr& layer : layers)
            {
                layer->draw();
            }
        }

        void Scene::addLayer(const LayerPtr& layer)
        {
            if (!hasLayer(layer))
            {
                layers.push_back(layer);

                if (entered) layer->enter();
            }
        }

        void Scene::removeLayer(const LayerPtr& layer)
        {
            std::vector<LayerPtr>::iterator i = std::find(layers.begin(), layers.end(), layer);

            if (i != layers.end())
            {
                if (entered)
                {
                    layer->leave();
                }

                layers.erase(i);
            }
        }

        void Scene::removeAllLayers()
        {
            if (entered)
            {
                for (const LayerPtr& layer : layers)
                {
                    layer->leave();
                }
            }

            layers.clear();
        }

        bool Scene::hasLayer(const LayerPtr& layer) const
        {
            std::vector<LayerPtr>::const_iterator i = std::find(layers.begin(), layers.end(), layer);

            return i != layers.end();
        }

        void Scene::recalculateProjection()
        {
            for (const LayerPtr& layer : layers)
            {
                layer->recalculateProjection();
            }
        }

        NodePtr Scene::pickNode(const Vector2& position) const
        {
            for (std::vector<LayerPtr>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                const LayerPtr& layer = *i;

                if (NodePtr result = layer->pickNode(position))
                {
                    return result;
                }
            }

            return nullptr;
        }

        std::vector<NodePtr> Scene::pickNodes(const Vector2& position) const
        {
            std::vector<NodePtr> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<NodePtr> nodes = (*i)->pickNodes(position);

                result.insert(result.end(), nodes.begin(), nodes.end());
            }

            return result;
        }

        std::vector<NodePtr> Scene::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::vector<NodePtr> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<NodePtr> nodes = (*i)->pickNodes(edges);

                result.insert(result.end(), nodes.begin(), nodes.end());
            }

            return result;
        }

        void Scene::enter()
        {
            entered = true;

            recalculateProjection();
            sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

            for (const LayerPtr& layer : layers)
            {
                layer->enter();
            }
        }

        void Scene::leave()
        {
            entered = false;

            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);

            for (const LayerPtr& layer : layers)
            {
                layer->leave();
            }
        }

        bool Scene::handleWindow(Event::Type type, const WindowEvent&)
        {
            if (type == Event::Type::WINDOW_RESOLUTION_CHANGE)
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
                    scene::NodePtr node = pickNode(event.position);
                    pointerDownOnNode(0, node, event.position);
                    break;
                }
                case Event::Type::MOUSE_UP:
                {
                    scene::NodePtr node = pickNode(event.position);
                    pointerUpOnNode(0, node, event.position);
                    break;
                }
                case Event::Type::MOUSE_MOVE:
                {
                    scene::NodePtr previousNode = pickNode(event.previousPosition);
                    pointerLeaveNode(0, previousNode, event.position);

                    scene::NodePtr node = pickNode(event.position);
                    pointerEnterNode(0, node, event.position);

                    auto i = pointerDownOnNodes.find(0);

                    if (i != pointerDownOnNodes.end())
                    {
                        pointerDragNode(0, i->second, event.position);
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
                    scene::NodePtr node = pickNode(event.position);
                    pointerDownOnNode(event.touchId, node, event.position);
                    break;
                }
                case Event::Type::TOUCH_END:
                {
                    scene::NodePtr node = pickNode(event.position);
                    pointerUpOnNode(event.touchId, node, event.position);
                    break;
                }
                case Event::Type::TOUCH_MOVE:
                {
                    scene::NodePtr previousNode = pickNode(event.previousPosition);
                    pointerLeaveNode(0, previousNode, event.position);

                    scene::NodePtr node = pickNode(event.position);
                    pointerEnterNode(0, node, event.position);

                    auto i = pointerDownOnNodes.find(event.touchId);

                    if (i != pointerDownOnNodes.end())
                    {
                        pointerDragNode(event.touchId, i->second, event.position);
                    }
                    break;
                }
                case Event::Type::TOUCH_CANCEL:
                {
                    scene::NodePtr node = pickNode(event.position);
                    pointerUpOnNode(event.touchId, node, event.position);
                    break;
                }
                default:
                    break;
            }

            return true;
        }

        void Scene::pointerEnterNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
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

        void Scene::pointerLeaveNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
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

        void Scene::pointerDownOnNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
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

        void Scene::pointerUpOnNode(uint64_t pointerId, const scene::NodePtr& node, const Vector2& position)
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

        void Scene::pointerDragNode(uint64_t, const scene::NodePtr& node, const Vector2& position)
        {
            if (node)
            {
                Event event;
                event.type = Event::Type::UI_DRAG_NODE;

                event.uiEvent.node = node;
                event.uiEvent.position = position;

                sharedEngine->getEventDispatcher()->postEvent(event);
            }
        }
    } // namespace scene
} // namespace ouzel
