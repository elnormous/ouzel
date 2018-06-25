// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include <cassert>
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
        }

        void Layer::draw()
        {
            for (Camera* camera : cameras)
            {
                std::vector<Actor*> drawQueue;

                for (Actor* actor : children)
                    actor->visit(drawQueue, Matrix4::identity(), false, camera, 0, false);

                engine->getRenderer()->addSetRenderTargetCommand(camera->getRenderTarget());
                engine->getRenderer()->addSetViewportCommand(camera->getRenderViewport());
                engine->getRenderer()->addSetDepthStateCommand(camera->getDepthTest(), camera->getDepthWrite());

                for (Actor* actor : drawQueue)
                {
                    actor->draw(camera, false);

                    if (camera->getWireframe())
                    {
                        engine->getRenderer()->addSetFillModeCommad(graphics::Renderer::FillMode::WIREFRAME);
                        actor->draw(camera, true);
                        engine->getRenderer()->addSetFillModeCommad(graphics::Renderer::FillMode::SOLID);
                    }
                }
            }
        }

        void Layer::addChild(Actor* actor)
        {
            ActorContainer::addChild(actor);

            if (actor)
                actor->updateTransform(Matrix4::identity());
        }

        void Layer::addCamera(Camera* camera)
        {
            assert(camera);

            cameras.push_back(camera);
        }

        void Layer::removeCamera(Camera* camera)
        {
            auto i = std::find(cameras.begin(), cameras.end(), camera);

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
            auto i = std::find(lights.begin(), lights.end(), light);

            if (i != lights.end())
                lights.erase(i);
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
                        worldEdges.push_back(camera->convertNormalizedToWorld(edge));

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
                camera->recalculateProjection();
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
