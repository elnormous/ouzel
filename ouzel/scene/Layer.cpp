// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Layer.hpp"
#include "core/Engine.hpp"
#include "Actor.hpp"
#include "Camera.hpp"
#include "graphics/Renderer.hpp"
#include "Scene.hpp"
#include "math/Matrix4.hpp"
#include "Component.hpp"

namespace ouzel
{
    namespace scene
    {
        Layer::Layer()
        {
            layer = this;
        }

        Layer::~Layer()
        {
            if (scene) scene->removeLayer(this);

            for (Camera* camera : cameras)
            {
                camera->addedToLayer = nullptr;
            }
        }

        void Layer::draw()
        {
            for (Camera* camera : cameras)
            {
                std::vector<Actor*> drawQueue;

                for (Actor* actor : children)
                {
                    actor->visit(drawQueue, Matrix4::IDENTITY, false, camera, 0, false);
                }

                for (Actor* actor : drawQueue)
                {
                    actor->draw(camera, false);

                    if (camera->getWireframe())
                    {
                        actor->draw(camera, true);
                    }
                }
            }
        }

        void Layer::addChildActor(Actor* actor)
        {
            ActorContainer::addChildActor(actor);

            if (actor)
            {
                actor->updateTransform(Matrix4::IDENTITY);
            }
        }

        void Layer::addCamera(Camera* camera)
        {
            cameras.push_back(camera);
            camera->recalculateProjection();
        }

        void Layer::removeCamera(Camera* camera)
        {
            auto i = std::find(cameras.begin(), cameras.end(), camera);

            if (i != cameras.end())
            {
                cameras.erase(i);
            }
        }

        std::pair<Actor*, ouzel::Vector3> Layer::pickActor(const Vector2& position, bool renderTargets) const
        {
            for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
            {
                Camera* camera = *i;

                if (renderTargets || !camera->getRenderTarget())
                {
                    std::vector<std::pair<Actor*, ouzel::Vector3>> actors;

                    Vector2 worldPosition = camera->convertNormalizedToWorld(position);

                    findActors(worldPosition, actors);

                    if (!actors.empty()) return actors.front();
                }
            }

            return std::make_pair(nullptr, Vector3());
        }

        std::vector<std::pair<Actor*, ouzel::Vector3>> Layer::pickActors(const Vector2& position, bool renderTargets) const
        {
            std::vector<std::pair<Actor*, ouzel::Vector3>> result;

            for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
            {
                Camera* camera = *i;

                if (renderTargets || !camera->getRenderTarget())
                {
                    Vector2 worldPosition = camera->convertNormalizedToWorld(position);

                    std::vector<std::pair<Actor*, ouzel::Vector3>> actors;
                    findActors(worldPosition, actors);

                    result.insert(result.end(), actors.begin(), actors.end());
                }
            }

            return result;
        }

        std::vector<Actor*> Layer::pickActors(const std::vector<Vector2>& edges, bool renderTargets) const
        {
            std::vector<Actor*> result;

            for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
            {
                Camera* camera = *i;

                if (renderTargets || !camera->getRenderTarget())
                {
                    std::vector<Vector2> worldEdges;
                    worldEdges.reserve(edges.size());

                    for (const Vector2& edge : edges)
                    {
                        worldEdges.push_back(camera->convertNormalizedToWorld(edge));
                    }

                    std::vector<Actor*> actors;
                    findActors(worldEdges, actors);

                    result.insert(result.end(), actors.begin(), actors.end());
                }
            }

            return result;
        }

        void Layer::setOrder(int32_t newOrder)
        {
            order = newOrder;
        }

        void Layer::recalculateProjection()
        {
            for (Camera* camera : cameras)
            {
                camera->recalculateProjection();
            }
        }

        void Layer::enter()
        {
            ActorContainer::enter();

            recalculateProjection();
        }

        void Layer::removeFromScene()
        {
            if (scene) scene->removeLayer(this);
        }
    } // namespace scene
} // namespace ouzel
