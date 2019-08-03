// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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

            std::set<graphics::RenderTarget*> clearedRenderTargets;

            for (Layer* layer : layers)
            {
                // clear all the render targets
                for (Camera* camera : layer->getCameras())
                {
                    if ((camera->getClearColorBuffer() || camera->getClearDepthBuffer() || camera->getClearStencilBuffer()) &&
                        clearedRenderTargets.insert(camera->getRenderTarget()).second)
                    {
                        engine->getRenderer()->setRenderTarget(camera->getRenderTarget() ? camera->getRenderTarget()->getResource() : 0);
                        engine->getRenderer()->clearRenderTarget(camera->getClearColorBuffer(),
                                                                 camera->getClearDepthBuffer(),
                                                                 camera->getClearStencilBuffer(),
                                                                 camera->getClearColor(),
                                                                 camera->getClearDepth(),
                                                                 camera->getClearStencil());
                    }
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

            auto ownedIterator = std::find_if(ownedLayers.begin(), ownedLayers.end(), [layer](const auto& other) {
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

        std::pair<Actor*, Vector3F> Scene::pickActor(const Vector2F& position, bool renderTargets) const
        {
            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                const Layer* layer = *i;
                std::pair<Actor*, Vector3F> result = layer->pickActor(position, renderTargets);

                if (result.first) return result;
            }

            return std::make_pair(nullptr, Vector3F());
        }

        std::vector<std::pair<Actor*, Vector3F>> Scene::pickActors(const Vector2F& position, bool renderTargets) const
        {
            std::vector<std::pair<Actor*, Vector3F>> result;

            for (auto i = layers.rbegin(); i != layers.rend(); ++i)
            {
                auto actors = (*i)->pickActors(position, renderTargets);

                result.insert(result.end(), actors.begin(), actors.end());
            }

            return result;
        }

        std::vector<Actor*> Scene::pickActors(const std::vector<Vector2F>& edges, bool renderTargets) const
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
            engine->getEventDispatcher().addEventHandler(eventHandler);

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
            if (event.type == Event::Type::ResolutionChange)
                recalculateProjection();

            return false;
        }

        bool Scene::handleMouse(const MouseEvent& event)
        {
            switch (event.type)
            {
                case Event::Type::MousePress:
                {
                    std::pair<Actor*, Vector3F> actor = pickActor(event.position);
                    pointerDownOnActor(0, actor.first, event.position, actor.second);
                    break;
                }
                case Event::Type::MouseRelease:
                {
                    std::pair<Actor*, Vector3F> actor = pickActor(event.position);
                    pointerUpOnActor(0, actor.first, event.position);
                    break;
                }
                case Event::Type::MouseMove:
                {
                    std::pair<Actor*, Vector3F> previousActor = pickActor(event.position - event.difference);
                    pointerLeaveActor(0, previousActor.first, event.position);

                    std::pair<Actor*, Vector3F> actor = pickActor(event.position);
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
                    case Event::Type::TouchBegin:
                    {
                        std::pair<Actor*, Vector3F> actor = pickActor(event.position);
                        pointerDownOnActor(event.touchId, actor.first, event.position, actor.second);
                        break;
                    }
                    case Event::Type::TouchEnd:
                    {
                        std::pair<Actor*, Vector3F> actor = pickActor(event.position);
                        pointerUpOnActor(event.touchId, actor.first, event.position);
                        break;
                    }
                    case Event::Type::TouchMove:
                    {
                        std::pair<Actor*, Vector3F> previousActor = pickActor(event.position - event.difference);
                        pointerLeaveActor(0, previousActor.first, event.position);

                        std::pair<Actor*, Vector3F> actor = pickActor(event.position);
                        pointerEnterActor(0, actor.first, event.position);

                        auto i = pointerDownOnActors.find(event.touchId);

                        if (i != pointerDownOnActors.end())
                            pointerDragActor(event.touchId, i->second.first, event.position, event.difference, i->second.second);
                        break;
                    }
                    case Event::Type::TouchCancel:
                    {
                        std::pair<Actor*, Vector3F> actor = pickActor(event.position);
                        pointerUpOnActor(event.touchId, actor.first, event.position);
                        break;
                    }
                    default:
                        break;
                }
            }

            return false;
        }

        void Scene::pointerEnterActor(uint64_t pointerId, Actor* actor, const Vector2F& position)
        {
            if (actor)
            {
                auto event = std::make_unique<UIEvent>();
                event->type = Event::Type::ActorEnter;
                event->actor = actor;
                event->touchId = pointerId;
                event->position = position;
                engine->getEventDispatcher().dispatchEvent(std::move(event));
            }
        }

        void Scene::pointerLeaveActor(uint64_t pointerId, Actor* actor, const Vector2F& position)
        {
            if (actor)
            {
                auto event = std::make_unique<UIEvent>();
                event->type = Event::Type::ActorLeave;
                event->actor = actor;
                event->touchId = pointerId;
                event->position = position;
                engine->getEventDispatcher().dispatchEvent(std::move(event));
            }
        }

        void Scene::pointerDownOnActor(uint64_t pointerId, Actor* actor, const Vector2F& position, const Vector3F& localPosition)
        {
            if (actor)
            {
                pointerDownOnActors[pointerId] = std::make_pair(actor, localPosition);

                auto event = std::make_unique<UIEvent>();
                event->type = Event::Type::ActorPress;
                event->actor = actor;
                event->touchId = pointerId;
                event->position = position;
                event->localPosition = localPosition;
                engine->getEventDispatcher().dispatchEvent(std::move(event));
            }
        }

        void Scene::pointerUpOnActor(uint64_t pointerId, Actor* actor, const Vector2F& position)
        {
            auto i = pointerDownOnActors.find(pointerId);

            if (i != pointerDownOnActors.end())
            {
                const auto pointerDownOnActor = i->second;

                if (pointerDownOnActor.first)
                {
                    auto releaseEvent = std::make_unique<UIEvent>();
                    releaseEvent->type = Event::Type::ActorRelease;
                    releaseEvent->actor = pointerDownOnActor.first;
                    releaseEvent->touchId = pointerId;
                    releaseEvent->position = position;
                    releaseEvent->localPosition = pointerDownOnActor.second;

                    engine->getEventDispatcher().dispatchEvent(std::move(releaseEvent));

                    if (pointerDownOnActor.first == actor)
                    {
                        auto clickEvent = std::make_unique<UIEvent>();
                        clickEvent->type = Event::Type::ActorClick;
                        clickEvent->actor = actor;
                        clickEvent->touchId = pointerId;
                        clickEvent->position = position;

                        engine->getEventDispatcher().dispatchEvent(std::move(clickEvent));
                    }
                }
            }

            pointerDownOnActors.erase(pointerId);
        }

        void Scene::pointerDragActor(uint64_t pointerId, Actor* actor, const Vector2F& position,
                                     const Vector2F& difference, const Vector3F& localPosition)
        {
            if (actor)
            {
                auto event = std::make_unique<UIEvent>();
                event->type = Event::Type::ActorDrag;
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
