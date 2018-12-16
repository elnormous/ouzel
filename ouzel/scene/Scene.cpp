// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
            eventHandler.windowHandler = std::bind(&Scene::handleWindow, this, std::placeholders::_1);
            eventHandler.mouseHandler = std::bind(&Scene::handleMouse, this, std::placeholders::_1);
            eventHandler.touchHandler = std::bind(&Scene::handleTouch, this, std::placeholders::_1);
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

            std::set<uint64_t> clearedRenderTargets;

            for (Layer* layer : layers)
            {
                // clear all the render targets
                for (Camera* camera : layer->getCameras())
                {
                    uintptr_t renderTarget = camera->getRenderTarget() ? camera->getRenderTarget()->getResource() : 0;

                    if (clearedRenderTargets.insert(renderTarget).second)
                        engine->getRenderer()->clearRenderTarget(renderTarget);
                }

                layer->draw();
            }

            engine->getRenderer()->present();
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

            auto layerIterator = std::find(layers.begin(), layers.end(), layer);

            if (layerIterator != layers.end())
            {
                if (entered)
                    layer->leave();

                layers.erase(layerIterator);

                layer->scene = nullptr;

                result = true;
            }

            auto ownedIterator = std::find_if(ownedLayers.begin(), ownedLayers.end(), [layer](const std::unique_ptr<Layer>& other) {
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
            auto i = std::find(layers.cbegin(), layers.cend(), layer);

            return i != layers.end();
        }

        void Scene::recalculateProjection()
        {
            for (Layer* layer : layers)
                layer->recalculateProjection();
        }

        std::pair<Actor*, Vector3> Scene::pickActor(const Vector2& position, bool renderTargets) const
        {
            for (std::vector<Layer*>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                Layer* layer = *i;
                std::pair<Actor*, Vector3> result = layer->pickActor(position, renderTargets);

                if (result.first) return result;
            }

            return std::make_pair(nullptr, Vector3());
        }

        std::vector<std::pair<Actor*, Vector3>> Scene::pickActors(const Vector2& position, bool renderTargets) const
        {
            std::vector<std::pair<Actor*, Vector3>> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::vector<std::pair<Actor*, Vector3>> actors = (*i)->pickActors(position, renderTargets);

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
            engine->getEventDispatcher().addEventHandler(&eventHandler);

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

        bool Scene::handleWindow(const WindowEvent& event)
        {
            if (event.type == Event::Type::RESOLUTION_CHANGE)
                recalculateProjection();

            return false;
        }

        bool Scene::handleMouse(const MouseEvent& event)
        {
            switch (event.type)
            {
                case Event::Type::MOUSE_PRESS:
                {
                    std::pair<Actor*, Vector3> actor = pickActor(event.position);
                    pointerDownOnActor(0, actor.first, event.position, actor.second);
                    break;
                }
                case Event::Type::MOUSE_RELEASE:
                {
                    std::pair<Actor*, Vector3> actor = pickActor(event.position);
                    pointerUpOnActor(0, actor.first, event.position);
                    break;
                }
                case Event::Type::MOUSE_MOVE:
                {
                    std::pair<Actor*, Vector3> previousActor = pickActor(event.position - event.difference);
                    pointerLeaveActor(0, previousActor.first, event.position);

                    std::pair<Actor*, Vector3> actor = pickActor(event.position);
                    pointerEnterActor(0, actor.first, event.position);

                    auto i = pointerDownOnActors.find(0);

                    if (i != pointerDownOnActors.end())
                        pointerDragActor(0, i->second.first, event.position, event.difference, i->second.second);
                    break;
                }
                default:
                    break;
            }

            return false;
        }

        bool Scene::handleTouch(const TouchEvent& event)
        {
            if (event.touchpad->isScreen())
            {
                switch (event.type)
                {
                    case Event::Type::TOUCH_BEGIN:
                    {
                        std::pair<Actor*, Vector3> actor = pickActor(event.position);
                        pointerDownOnActor(event.touchId, actor.first, event.position, actor.second);
                        break;
                    }
                    case Event::Type::TOUCH_END:
                    {
                        std::pair<Actor*, Vector3> actor = pickActor(event.position);
                        pointerUpOnActor(event.touchId, actor.first, event.position);
                        break;
                    }
                    case Event::Type::TOUCH_MOVE:
                    {
                        std::pair<Actor*, Vector3> previousActor = pickActor(event.position - event.difference);
                        pointerLeaveActor(0, previousActor.first, event.position);

                        std::pair<Actor*, Vector3> actor = pickActor(event.position);
                        pointerEnterActor(0, actor.first, event.position);

                        auto i = pointerDownOnActors.find(event.touchId);

                        if (i != pointerDownOnActors.end())
                            pointerDragActor(event.touchId, i->second.first, event.position, event.difference, i->second.second);
                        break;
                    }
                    case Event::Type::TOUCH_CANCEL:
                    {
                        std::pair<Actor*, Vector3> actor = pickActor(event.position);
                        pointerUpOnActor(event.touchId, actor.first, event.position);
                        break;
                    }
                    default:
                        break;
                }
            }

            return false;
        }

        void Scene::pointerEnterActor(uint64_t pointerId, Actor* actor, const Vector2& position)
        {
            if (actor)
            {
                std::unique_ptr<UIEvent> event(new UIEvent());
                event->type = Event::Type::ACTOR_ENTER;
                event->actor = actor;
                event->touchId = pointerId;
                event->position = position;
                engine->getEventDispatcher().dispatchEvent(std::move(event));
            }
        }

        void Scene::pointerLeaveActor(uint64_t pointerId, Actor* actor, const Vector2& position)
        {
            if (actor)
            {
                std::unique_ptr<UIEvent> event(new UIEvent());
                event->type = Event::Type::ACTOR_LEAVE;
                event->actor = actor;
                event->touchId = pointerId;
                event->position = position;
                engine->getEventDispatcher().dispatchEvent(std::move(event));
            }
        }

        void Scene::pointerDownOnActor(uint64_t pointerId, Actor* actor, const Vector2& position, const Vector3& localPosition)
        {
            if (actor)
            {
                pointerDownOnActors[pointerId] = std::make_pair(actor, localPosition);

                std::unique_ptr<UIEvent> event(new UIEvent());
                event->type = Event::Type::ACTOR_PRESS;
                event->actor = actor;
                event->touchId = pointerId;
                event->position = position;
                event->localPosition = localPosition;
                engine->getEventDispatcher().dispatchEvent(std::move(event));
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
                    std::unique_ptr<UIEvent> releaseEvent(new UIEvent());
                    releaseEvent->type = Event::Type::ACTOR_RELEASE;
                    releaseEvent->actor = pointerDownOnActor.first;
                    releaseEvent->touchId = pointerId;
                    releaseEvent->position = position;
                    releaseEvent->localPosition = pointerDownOnActor.second;

                    engine->getEventDispatcher().dispatchEvent(std::move(releaseEvent));

                    if (pointerDownOnActor.first == actor)
                    {
                        std::unique_ptr<UIEvent> clickEvent(new UIEvent());
                        clickEvent->type = Event::Type::ACTOR_CLICK;
                        clickEvent->actor = actor;
                        clickEvent->touchId = pointerId;
                        clickEvent->position = position;

                        engine->getEventDispatcher().dispatchEvent(std::move(clickEvent));
                    }
                }
            }

            pointerDownOnActors.erase(pointerId);
        }

        void Scene::pointerDragActor(uint64_t pointerId, Actor* actor, const Vector2& position,
                                     const Vector2& difference, const Vector3& localPosition)
        {
            if (actor)
            {
                std::unique_ptr<UIEvent> event(new UIEvent());
                event->type = Event::Type::ACTOR_DRAG;
                event->actor = actor;
                event->touchId = pointerId;
                event->difference = difference;
                event->position = position;
                event->localPosition = localPosition;
                engine->getEventDispatcher().dispatchEvent(std::move(event));
            }
        }
    } // namespace scene
} // namespace ouzel
