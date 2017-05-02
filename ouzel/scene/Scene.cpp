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
            for (const std::shared_ptr<Layer>& layer : layers)
            {
                if (entered) layer->leave();
                layer->scene = nullptr;
            }
        }

        void Scene::draw()
        {
            std::stable_sort(layers.begin(), layers.end(), [](const std::shared_ptr<Layer>& a, const std::shared_ptr<Layer>& b) {
                return a->getOrder() > b->getOrder();
            });

            for (const std::shared_ptr<Layer>& layer : layers)
            {
                layer->draw();
            }
        }

        void Scene::addLayer(const std::shared_ptr<Layer>& layer)
        {
            if (layer && !hasLayer(layer))
            {
                layers.push_back(layer);

                if (entered) layer->enter();

                layer->scene = this;
            }
        }

        void Scene::removeLayer(const std::shared_ptr<Layer>& layer)
        {
            std::vector<std::shared_ptr<Layer>>::iterator i = std::find(layers.begin(), layers.end(), layer);

            if (i != layers.end())
            {
                if (entered)
                {
                    layer->leave();
                }

                layers.erase(i);

                layer->scene = nullptr;
            }
        }

        void Scene::removeAllLayers()
        {
            if (entered)
            {
                for (const std::shared_ptr<Layer>& layer : layers)
                {
                    layer->leave();
                }
            }

            layers.clear();
        }

        bool Scene::hasLayer(const std::shared_ptr<Layer>& layer) const
        {
            std::vector<std::shared_ptr<Layer>>::const_iterator i = std::find(layers.begin(), layers.end(), layer);

            return i != layers.end();
        }

        void Scene::recalculateProjection()
        {
            for (const std::shared_ptr<Layer>& layer : layers)
            {
                layer->recalculateProjection();
            }
        }

        std::shared_ptr<Node> Scene::pickNode(const Vector2& position) const
        {
            for (std::vector<std::shared_ptr<Layer>>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                const std::shared_ptr<Layer>& layer = *i;

                if (std::shared_ptr<Node> result = layer->pickNode(position))
                {
                    return result;
                }
            }

            return nullptr;
        }

        std::vector<std::shared_ptr<Node>> Scene::pickNodes(const Vector2& position) const
        {
            std::vector<std::shared_ptr<Node>> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<std::shared_ptr<Node>> nodes = (*i)->pickNodes(position);

                result.insert(result.end(), nodes.begin(), nodes.end());
            }

            return result;
        }

        std::vector<std::shared_ptr<Node>> Scene::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::vector<std::shared_ptr<Node>> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<std::shared_ptr<Node>> nodes = (*i)->pickNodes(edges);

                result.insert(result.end(), nodes.begin(), nodes.end());
            }

            return result;
        }

        void Scene::enter()
        {
            entered = true;

            recalculateProjection();
            sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

            for (const std::shared_ptr<Layer>& layer : layers)
            {
                layer->enter();
            }
        }

        void Scene::leave()
        {
            entered = false;

            sharedEngine->getEventDispatcher()->removeEventHandler(&eventHandler);

            for (const std::shared_ptr<Layer>& layer : layers)
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
                    std::shared_ptr<Node> node = pickNode(event.position);
                    pointerDownOnNode(0, node, event.position);
                    break;
                }
                case Event::Type::MOUSE_UP:
                {
                    std::shared_ptr<Node> node = pickNode(event.position);
                    pointerUpOnNode(0, node, event.position);
                    break;
                }
                case Event::Type::MOUSE_MOVE:
                {
                    std::shared_ptr<Node> previousNode = pickNode(event.position - event.difference);
                    pointerLeaveNode(0, previousNode, event.position);

                    std::shared_ptr<scene::Node> node = pickNode(event.position);
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
                    std::shared_ptr<Node> node = pickNode(event.position);
                    pointerDownOnNode(event.touchId, node, event.position);
                    break;
                }
                case Event::Type::TOUCH_END:
                {
                    std::shared_ptr<Node> node = pickNode(event.position);
                    pointerUpOnNode(event.touchId, node, event.position);
                    break;
                }
                case Event::Type::TOUCH_MOVE:
                {
                    std::shared_ptr<Node> previousNode = pickNode(event.position - event.difference);
                    pointerLeaveNode(0, previousNode, event.position);

                    std::shared_ptr<Node> node = pickNode(event.position);
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
                    std::shared_ptr<Node> node = pickNode(event.position);
                    pointerUpOnNode(event.touchId, node, event.position);
                    break;
                }
                default:
                    break;
            }

            return true;
        }

        void Scene::pointerEnterNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position)
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

        void Scene::pointerLeaveNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position)
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

        void Scene::pointerDownOnNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position)
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

        void Scene::pointerUpOnNode(uint64_t pointerId, const std::shared_ptr<Node>& node, const Vector2& position)
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

        void Scene::pointerDragNode(uint64_t, const std::shared_ptr<Node>& node, const Vector2& position, const Vector2& difference)
        {
            if (node)
            {
                Event event;
                event.type = Event::Type::UI_DRAG_NODE;

                event.uiEvent.node = node;
                event.uiEvent.difference = difference;
                event.uiEvent.position = position;

                sharedEngine->getEventDispatcher()->postEvent(event);
            }
        }
    } // namespace scene
} // namespace ouzel
