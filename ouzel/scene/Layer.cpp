// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Layer.hpp"
#include "core/Engine.hpp"
#include "Node.hpp"
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
                std::vector<Node*> drawQueue;

                for (Node* node : children)
                {
                    node->visit(drawQueue, Matrix4::IDENTITY, false, camera, 0, false);
                }

                for (Node* node : drawQueue)
                {
                    node->draw(camera, false);

                    if (camera->getWireframe())
                    {
                        node->draw(camera, true);
                    }
                }
            }
        }

        void Layer::addChildNode(Node* node)
        {
            NodeContainer::addChildNode(node);

            if (node)
            {
                node->updateTransform(Matrix4::IDENTITY);
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

        std::pair<Node*, ouzel::Vector3> Layer::pickNode(const Vector2& position, bool renderTargets) const
        {
            for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
            {
                Camera* camera = *i;

                if (renderTargets || !camera->getRenderTarget())
                {
                    std::vector<std::pair<Node*, ouzel::Vector3>> nodes;

                    Vector2 worldPosition = camera->convertNormalizedToWorld(position);

                    findNodes(worldPosition, nodes);

                    if (!nodes.empty()) return nodes.front();
                }
            }

            return std::make_pair(nullptr, Vector3());
        }

        std::vector<std::pair<Node*, ouzel::Vector3>> Layer::pickNodes(const Vector2& position, bool renderTargets) const
        {
            std::vector<std::pair<Node*, ouzel::Vector3>> result;

            for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
            {
                Camera* camera = *i;

                if (renderTargets || !camera->getRenderTarget())
                {
                    Vector2 worldPosition = camera->convertNormalizedToWorld(position);

                    std::vector<std::pair<Node*, ouzel::Vector3>> nodes;
                    findNodes(worldPosition, nodes);

                    result.insert(result.end(), nodes.begin(), nodes.end());
                }
            }

            return result;
        }

        std::vector<Node*> Layer::pickNodes(const std::vector<Vector2>& edges, bool renderTargets) const
        {
            std::vector<Node*> result;

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

                    std::vector<Node*> nodes;
                    findNodes(worldEdges, nodes);

                    result.insert(result.end(), nodes.begin(), nodes.end());
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
            NodeContainer::enter();

            recalculateProjection();
        }

        void Layer::removeFromScene()
        {
            if (scene) scene->removeLayer(this);
        }
    } // namespace scene
} // namespace ouzel
