// Ouzel by Elviss Strazdins

#include <cassert>
#include <algorithm>
#include "Scene.hpp"
#include "Layer.hpp"
#include "Camera.hpp"
#include "SceneManager.hpp"
#include "../core/Engine.hpp"
#include "../events/EventDispatcher.hpp"

namespace ouzel::scene
{
    Scene::Scene():
        eventHandler{EventHandler::priorityMax + 1}
    {
        eventHandler.windowHandler = std::bind(&Scene::handleWindow, this, std::placeholders::_1);
        eventHandler.mouseHandler = std::bind(&Scene::handleMouse, this, std::placeholders::_1);
        eventHandler.touchHandler = std::bind(&Scene::handleTouch, this, std::placeholders::_1);
    }

    Scene::~Scene()
    {
        if (sceneManger) sceneManger->removeScene(*this);

        for (auto layer : layers)
        {
            if (entered) layer->leave();
            layer->scene = nullptr;
        }
    }

    void Scene::draw()
    {
        std::stable_sort(layers.begin(), layers.end(), [](const auto a, const auto b) noexcept {
            return a->getOrder() > b->getOrder();
        });

        std::set<graphics::RenderTarget*> clearedRenderTargets;

        for (const auto layer : layers)
        {
            // clear all the render targets
            for (const auto camera : layer->getCameras())
            {
                if ((camera->getClearColorBuffer() || camera->getClearDepthBuffer() || camera->getClearStencilBuffer()) &&
                    clearedRenderTargets.insert(camera->getRenderTarget()).second)
                {
                    engine->getGraphics()->setRenderTarget(camera->getRenderTarget() ? camera->getRenderTarget()->getResource() : 0);
                    engine->getGraphics()->clearRenderTarget(camera->getClearColorBuffer(),
                                                             camera->getClearDepthBuffer(),
                                                             camera->getClearStencilBuffer(),
                                                             camera->getClearColor(),
                                                             camera->getClearDepth(),
                                                             camera->getClearStencil());
                }
            }

            layer->draw();
        }

        engine->getGraphics()->present();
    }

    void Scene::addLayer(Layer& layer)
    {
        if (!hasLayer(layer))
        {
            layers.push_back(&layer);

            if (entered) layer.enter();

            layer.scene = this;
        }
    }

    bool Scene::removeLayer(const Layer& layer)
    {
        bool result = false;

        const auto layerIterator = std::find(layers.begin(), layers.end(), &layer);

        if (layerIterator != layers.end())
        {
            Layer* child = *layerIterator;

            if (entered)
                child->leave();

            child->scene = nullptr;

            layers.erase(layerIterator);

            result = true;
        }

        const auto ownedIterator = std::find_if(ownedLayers.begin(), ownedLayers.end(), [&layer](const auto& other) noexcept {
            return other.get() == &layer;
        });

        if (ownedIterator != ownedLayers.end())
            ownedLayers.erase(ownedIterator);

        return result;
    }

    void Scene::removeAllLayers()
    {
        if (entered)
        {
            for (auto layer : layers)
                layer->leave();
        }

        layers.clear();
        ownedLayers.clear();
    }

    bool Scene::hasLayer(const Layer& layer) const
    {
        return std::find(layers.cbegin(), layers.cend(), &layer) != layers.end();
    }

    void Scene::recalculateProjection()
    {
        for (auto layer : layers)
            layer->recalculateProjection();
    }

    std::pair<Actor*, Vector<float, 3>> Scene::pickActor(const Vector<float, 2>& position, bool renderTargets) const
    {
        for (auto i = layers.rbegin(); i != layers.rend(); ++i)
        {
            const auto layer = *i;
            std::pair<Actor*, Vector<float, 3>> result = layer->pickActor(position, renderTargets);

            if (result.first) return result;
        }

        return std::pair(nullptr, Vector<float, 3>{});
    }

    std::vector<std::pair<Actor*, Vector<float, 3>>> Scene::pickActors(const Vector<float, 2>& position, bool renderTargets) const
    {
        std::vector<std::pair<Actor*, Vector<float, 3>>> result;

        for (auto i = layers.rbegin(); i != layers.rend(); ++i)
        {
            auto actors = (*i)->pickActors(position, renderTargets);

            result.insert(result.end(), actors.begin(), actors.end());
        }

        return result;
    }

    std::vector<Actor*> Scene::pickActors(const std::vector<Vector<float, 2>>& edges, bool renderTargets) const
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

        for (auto layer : layers)
            layer->enter();
    }

    void Scene::leave()
    {
        entered = false;

        eventHandler.remove();

        for (auto layer : layers)
            layer->leave();
    }

    bool Scene::handleWindow(const WindowEvent& event)
    {
        if (event.type == Event::Type::resolutionChange)
            recalculateProjection();

        return false;
    }

    bool Scene::handleMouse(const MouseEvent& event)
    {
        switch (event.type)
        {
            case Event::Type::mousePress:
            {
                std::pair<Actor*, Vector<float, 3>> actor = pickActor(event.position);
                pointerDownOnActor(0, actor.first, event.position, actor.second);
                break;
            }
            case Event::Type::mouseRelease:
            {
                std::pair<Actor*, Vector<float, 3>> actor = pickActor(event.position);
                pointerUpOnActor(0, actor.first, event.position);
                break;
            }
            case Event::Type::mouseMove:
            {
                std::pair<Actor*, Vector<float, 3>> previousActor = pickActor(event.position - event.difference);
                pointerLeaveActor(0, previousActor.first, event.position);

                std::pair<Actor*, Vector<float, 3>> actor = pickActor(event.position);
                pointerEnterActor(0, actor.first, event.position);

                if (const auto i = pointerDownOnActors.find(0); i != pointerDownOnActors.end())
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
                case Event::Type::touchBegin:
                {
                    std::pair<Actor*, Vector<float, 3>> actor = pickActor(event.position);
                    pointerDownOnActor(event.touchId, actor.first, event.position, actor.second);
                    break;
                }
                case Event::Type::touchEnd:
                {
                    std::pair<Actor*, Vector<float, 3>> actor = pickActor(event.position);
                    pointerUpOnActor(event.touchId, actor.first, event.position);
                    break;
                }
                case Event::Type::touchMove:
                {
                    std::pair<Actor*, Vector<float, 3>> previousActor = pickActor(event.position - event.difference);
                    pointerLeaveActor(0, previousActor.first, event.position);

                    std::pair<Actor*, Vector<float, 3>> actor = pickActor(event.position);
                    pointerEnterActor(0, actor.first, event.position);

                    if (const auto i = pointerDownOnActors.find(event.touchId); i != pointerDownOnActors.end())
                        pointerDragActor(event.touchId, i->second.first, event.position, event.difference, i->second.second);
                    break;
                }
                case Event::Type::touchCancel:
                {
                    std::pair<Actor*, Vector<float, 3>> actor = pickActor(event.position);
                    pointerUpOnActor(event.touchId, actor.first, event.position);
                    break;
                }
                default:
                    break;
            }
        }

        return false;
    }

    void Scene::pointerEnterActor(std::uint64_t pointerId, Actor* actor, const Vector<float, 2>& position)
    {
        if (actor)
        {
            auto event = std::make_unique<UIEvent>();
            event->type = Event::Type::actorEnter;
            event->actor = actor;
            event->touchId = pointerId;
            event->position = position;
            engine->getEventDispatcher().dispatchEvent(std::move(event));
        }
    }

    void Scene::pointerLeaveActor(std::uint64_t pointerId, Actor* actor, const Vector<float, 2>& position)
    {
        if (actor)
        {
            auto event = std::make_unique<UIEvent>();
            event->type = Event::Type::actorLeave;
            event->actor = actor;
            event->touchId = pointerId;
            event->position = position;
            engine->getEventDispatcher().dispatchEvent(std::move(event));
        }
    }

    void Scene::pointerDownOnActor(std::uint64_t pointerId, Actor* actor, const Vector<float, 2>& position, const Vector<float, 3>& localPosition)
    {
        if (actor)
        {
            pointerDownOnActors[pointerId] = std::pair(actor, localPosition);

            auto event = std::make_unique<UIEvent>();
            event->type = Event::Type::actorPress;
            event->actor = actor;
            event->touchId = pointerId;
            event->position = position;
            event->localPosition = localPosition;
            engine->getEventDispatcher().dispatchEvent(std::move(event));
        }
    }

    void Scene::pointerUpOnActor(std::uint64_t pointerId, Actor* actor, const Vector<float, 2>& position)
    {
        if (const auto i = pointerDownOnActors.find(pointerId); i != pointerDownOnActors.end())
        {
            const auto pointerDownOnActor = i->second;

            if (pointerDownOnActor.first)
            {
                auto releaseEvent = std::make_unique<UIEvent>();
                releaseEvent->type = Event::Type::actorRelease;
                releaseEvent->actor = pointerDownOnActor.first;
                releaseEvent->touchId = pointerId;
                releaseEvent->position = position;
                releaseEvent->localPosition = pointerDownOnActor.second;

                engine->getEventDispatcher().dispatchEvent(std::move(releaseEvent));

                if (pointerDownOnActor.first == actor)
                {
                    auto clickEvent = std::make_unique<UIEvent>();
                    clickEvent->type = Event::Type::actorClick;
                    clickEvent->actor = actor;
                    clickEvent->touchId = pointerId;
                    clickEvent->position = position;

                    engine->getEventDispatcher().dispatchEvent(std::move(clickEvent));
                }
            }
        }

        pointerDownOnActors.erase(pointerId);
    }

    void Scene::pointerDragActor(std::uint64_t pointerId, Actor* actor, const Vector<float, 2>& position,
                                 const Vector<float, 2>& difference, const Vector<float, 3>& localPosition)
    {
        if (actor)
        {
            auto event = std::make_unique<UIEvent>();
            event->type = Event::Type::actorDrag;
            event->actor = actor;
            event->touchId = pointerId;
            event->difference = difference;
            event->position = position;
            event->localPosition = localPosition;
            engine->getEventDispatcher().dispatchEvent(std::move(event));
        }
    }
}
