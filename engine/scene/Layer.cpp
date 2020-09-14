// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <cassert>
#include <algorithm>
#include "Layer.hpp"
#include "Actor.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Component.hpp"
#include "../core/Engine.hpp"
#include "../graphics/Graphics.hpp"
#include "../math/Matrix.hpp"

namespace ouzel::scene
{
    Layer::Layer()
    {
        layer = this;
    }

    Layer::~Layer()
    {
        if (scene) scene->removeLayer(*this);
    }

    void Layer::draw()
    {
        for (const auto camera : cameras)
        {
            std::vector<Actor*> drawQueue;

            for (const auto actor : children)
                actor->visit(drawQueue, Matrix4F::identity(), false, camera, 0, false);

            engine->getGraphics()->setRenderTarget(camera->getRenderTarget() ? camera->getRenderTarget()->getResource() : 0);
            engine->getGraphics()->setViewport(camera->getRenderViewport());
            engine->getGraphics()->setDepthStencilState(camera->getDepthStencilState() ? camera->getDepthStencilState()->getResource() : 0,
                                                        camera->getStencilReferenceValue());

            for (const auto actor : drawQueue)
                actor->draw(camera, camera->getWireframe());
        }
    }

    void Layer::addChild(Actor* actor)
    {
        ActorContainer::addChild(actor);

        if (actor)
            actor->updateTransform(Matrix4F::identity());
    }

    void Layer::addCamera(Camera* camera)
    {
        assert(camera);

        cameras.push_back(camera);
    }

    void Layer::removeCamera(Camera* camera)
    {
        const auto i = std::find(cameras.begin(), cameras.end(), camera);

        if (i != cameras.end())
            cameras.erase(i);
    }

    void Layer::addLight(Light* light)
    {
        assert(light);

        lights.push_back(light);
    }

    void Layer::removeLight(Light* light)
    {
        const auto i = std::find(lights.begin(), lights.end(), light);

        if (i != lights.end())
            lights.erase(i);
    }

    std::pair<Actor*, Vector3F> Layer::pickActor(const Vector2F& position, bool renderTargets) const
    {
        for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
        {
            const auto camera = *i;

            if (renderTargets || !camera->getRenderTarget())
            {
                const auto worldPosition = Vector2F(camera->convertNormalizedToWorld(position));
                const auto actors = findActors(worldPosition);
                if (!actors.empty()) return actors.front();
            }
        }

        return std::make_pair(nullptr, Vector3F());
    }

    std::vector<std::pair<Actor*, Vector3F>> Layer::pickActors(const Vector2F& position, bool renderTargets) const
    {
        std::vector<std::pair<Actor*, Vector3F>> result;

        for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
        {
            const auto camera = *i;

            if (renderTargets || !camera->getRenderTarget())
            {
                const auto worldPosition = Vector2F(camera->convertNormalizedToWorld(position));
                const auto actors = findActors(worldPosition);
                result.insert(result.end(), actors.begin(), actors.end());
            }
        }

        return result;
    }

    std::vector<Actor*> Layer::pickActors(const std::vector<Vector2F>& edges, bool renderTargets) const
    {
        std::vector<Actor*> result;

        for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
        {
            const auto camera = *i;

            if (renderTargets || !camera->getRenderTarget())
            {
                std::vector<Vector2F> worldEdges;
                worldEdges.reserve(edges.size());

                for (const auto& edge : edges)
                    worldEdges.emplace_back(camera->convertNormalizedToWorld(edge));

                const auto actors = findActors(worldEdges);
                result.insert(result.end(), actors.begin(), actors.end());
            }
        }

        return result;
    }

    void Layer::setOrder(std::int32_t newOrder)
    {
        order = newOrder;
    }

    void Layer::recalculateProjection()
    {
        for (const auto camera : cameras)
            camera->recalculateProjection();
    }

    void Layer::enter()
    {
        ActorContainer::enter();

        recalculateProjection();
    }

    void Layer::removeFromScene()
    {
        if (scene) scene->removeLayer(*this);
    }
}
