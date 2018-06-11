// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>
#include <algorithm>
#include "Scene.hpp"
#include "Layer.hpp"
#include "Camera.hpp"
#include "SceneManager.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

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

            std::set<graphics::Texture*> clearedRenderTargets;

            for (Layer* layer : layers)
            {
                // clear all the render targets
                for (Camera* camera : layer->getCameras())
                {
                    if (clearedRenderTargets.insert(camera->getRenderTarget().get()).second)
                        engine->getRenderer()->addClearCommand(camera->getRenderTarget());
                }

                layer->draw();
            }
        }

        void Scene::addLayer(Layer* layer)
        {
            assert(layer);

            if (!hasLayer(layer))
            {
                layers.push_back(layer);

                if (entered) layer->enter();

                layer->scene = this;
            }
        }

        bool Scene::removeLayer(Layer* layer)
        {
            bool result = false;

            std::vector<Layer*>::iterator layerIterator = std::find(layers.begin(), layers.end(), layer);

            if (layerIterator != layers.end())
            {
                if (entered)
                    layer->leave();

                layers.erase(layerIterator);

                layer->scene = nullptr;

                result = true;
            }

            std::vector<std::unique_ptr<Layer>>::iterator ownedIterator = std::find_if(ownedLayers.begin(), ownedLayers.end(), [layer](const std::unique_ptr<Layer>& other) {
                return other.get() == layer;
            });

            if (ownedIterator != ownedLayers.end())
                ownedLayers.erase(ownedIterator);

            return result;
        }

        void Scene::removeAllLayers()
        {
            if (entered)
            {
                for (Layer* layer : layers)
                    layer->leave();
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
                layer->recalculateProjection();
        }

        std::pair<Actor*, ouzel::Vector3> Scene::pickActor(const Vector2& position, bool renderTargets) const
        {
            for (std::vector<Layer*>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                Layer* layer = *i;
                std::pair<Actor*, ouzel::Vector3> result = layer->pickActor(position, renderTargets);

                if (result.first) return result;
            }

            return std::make_pair(nullptr, Vector3());
        }

        std::vector<std::pair<Actor*, ouzel::Vector3>> Scene::pickActors(const Vector2& position, bool renderTargets) const
        {
            std::vector<std::pair<Actor*, ouzel::Vector3>> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<std::pair<Actor*, ouzel::Vector3>> actors = (*i)->pickActors(position, renderTargets);

                result.insert(result.end(), actors.begin(), actors.end());
            }

            return result;
        }

        std::vector<Actor*> Scene::pickActors(const std::vector<Vector2>& edges, bool renderTargets) const
        {
            std::vector<Actor*> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<Actor*> actors = (*i)->pickActors(edges, renderTargets);

                result.insert(result.end(), actors.begin(), actors.end());
            }

            return result;
        }

        void Scene::enter()
        {
            entered = true;

            recalculateProjection();
            engine->getEventDispatcher()->addEventHandler(&eventHandler);

            for (Layer* layer : layers)
                layer->enter();
        }

        void Scene::leave()
        {
            entered = false;

            eventHandler.remove();

            for (Layer* layer : layers)
                layer->leave();
        }

        bool Scene::handleWindow(Event::Type type, const WindowEvent&)
        {
            if (type == Event::Type::RESOLUTION_CHANGE)
                recalculateProjection();

            return true;
        }

        bool Scene::handleMouse(Event::Type type, const MouseEvent& event)
        {
            switch (type)
            {
                case Event::Type::MOUSE_PRESS:
                {
                    std::pair<Actor*, ouzel::Vector3> actor = pickActor(event.position);
                    pointerDownOnActor(0, actor.first, event.position, actor.second);
                    break;
                }
                case Event::Type::MOUSE_RELEASE:
                {
                    std::pair<Actor*, ouzel::Vector3> actor = pickActor(event.position);
                    pointerUpOnActor(0, actor.first, event.position);
                    break;
                }
                case Event::Type::MOUSE_MOVE:
                {
                    std::pair<Actor*, ouzel::Vector3> previousActor = pickActor(event.position - event.difference);
                    pointerLeaveActor(0, previousActor.first, event.position);

                    std::pair<Actor*, ouzel::Vector3> actor = pickActor(event.position);
                    pointerEnterActor(0, actor.first, event.position);

                    auto i = pointerDownOnActors.find(0);

                    if (i != pointerDownOnActors.end())
                        pointerDragActor(0, i->second.first, event.position, event.difference, i->second.second);
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
                    std::pair<Actor*, ouzel::Vector3> actor = pickActor(event.position);
                    pointerDownOnActor(event.touchId, actor.first, event.position, actor.second);
                    break;
                }
                case Event::Type::TOUCH_END:
                {
                    std::pair<Actor*, ouzel::Vector3> actor = pickActor(event.position);
                    pointerUpOnActor(event.touchId, actor.first, event.position);
                    break;
                }
                case Event::Type::TOUCH_MOVE:
                {
                    std::pair<Actor*, ouzel::Vector3> previousActor = pickActor(event.position - event.difference);
                    pointerLeaveActor(0, previousActor.first, event.position);

                    std::pair<Actor*, ouzel::Vector3> actor = pickActor(event.position);
                    pointerEnterActor(0, actor.first, event.position);

                    auto i = pointerDownOnActors.find(event.touchId);

                    if (i != pointerDownOnActors.end())
                        pointerDragActor(event.touchId, i->second.first, event.position, event.difference, i->second.second);
                    break;
                }
                case Event::Type::TOUCH_CANCEL:
                {
                    std::pair<Actor*, ouzel::Vector3> actor = pickActor(event.position);
                    pointerUpOnActor(event.touchId, actor.first, event.position);
                    break;
                }
                default:
                    break;
            }

            return true;
        }

        void Scene::pointerEnterActor(uint64_t pointerId, Actor* actor, const Vector2& position)
        {
            if (actor)
            {
                Event event;
                event.type = Event::Type::ACTOR_ENTER;

                event.uiEvent.actor = actor;
                event.uiEvent.touchId = pointerId;
                event.uiEvent.position = position;

                engine->getEventDispatcher()->postEvent(event);
            }
        }

        void Scene::pointerLeaveActor(uint64_t pointerId, Actor* actor, const Vector2& position)
        {
            if (actor)
            {
                Event event;
                event.type = Event::Type::ACTOR_LEAVE;

                event.uiEvent.actor = actor;
                event.uiEvent.touchId = pointerId;
                event.uiEvent.position = position;

                engine->getEventDispatcher()->postEvent(event);
            }
        }

        void Scene::pointerDownOnActor(uint64_t pointerId, Actor* actor, const Vector2& position, const Vector3& localPosition)
        {
            if (actor)
            {
                pointerDownOnActors[pointerId] = std::make_pair(actor, localPosition);

                Event event;
                event.type = Event::Type::ACTOR_PRESS;

                event.uiEvent.actor = actor;
                event.uiEvent.touchId = pointerId;
                event.uiEvent.position = position;
                event.uiEvent.localPosition = localPosition;

                engine->getEventDispatcher()->postEvent(event);
            }
        }

        void Scene::pointerUpOnActor(uint64_t pointerId, Actor* actor, const Vector2& position)
        {
            auto i = pointerDownOnActors.find(pointerId);

            if (i != pointerDownOnActors.end())
            {
                auto pointerDownOnActor = i->second;

                if (pointerDownOnActor.first)
                {
                    Event releaseEvent;
                    releaseEvent.type = Event::Type::ACTOR_RELEASE;

                    releaseEvent.uiEvent.actor = pointerDownOnActor.first;
                    releaseEvent.uiEvent.touchId = pointerId;
                    releaseEvent.uiEvent.position = position;
                    releaseEvent.uiEvent.localPosition = pointerDownOnActor.second;

                    engine->getEventDispatcher()->postEvent(releaseEvent);

                    if (pointerDownOnActor.first == actor)
                    {
                        Event clickEvent;
                        clickEvent.type = Event::Type::ACTOR_CLICK;

                        clickEvent.uiEvent.actor = actor;
                        clickEvent.uiEvent.touchId = pointerId;
                        clickEvent.uiEvent.position = position;

                        engine->getEventDispatcher()->postEvent(clickEvent);
                    }
                }
            }

            pointerDownOnActors.erase(pointerId);
        }

        void Scene::pointerDragActor(uint64_t pointerId, Actor* actor, const Vector2& position,
                                    const Vector2& difference, const ouzel::Vector3& localPosition)
        {
            if (actor)
            {
                Event event;
                event.type = Event::Type::ACTOR_DRAG;

                event.uiEvent.actor = actor;
                event.uiEvent.touchId = pointerId;
                event.uiEvent.difference = difference;
                event.uiEvent.position = position;
                event.uiEvent.localPosition = localPosition;

                engine->getEventDispatcher()->postEvent(event);
            }
        }
    } // namespace scene
} // namespace ouzel
