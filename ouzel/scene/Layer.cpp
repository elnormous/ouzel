// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Layer.h"
#include "core/Engine.h"
#include "Node.h"
#include "Camera.h"
#include "graphics/Renderer.h"
#include "Scene.h"
#include "math/Matrix4.h"
#include "Component.h"

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

        void Layer::addChild(Node* node)
        {
            NodeContainer::addChild(node);

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

        Node* Layer::pickNode(const Vector2& position) const
        {
            for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
            {
                Camera* camera = *i;

                std::vector<Node*> nodes;

                Vector2 worldPosition = camera->convertNormalizedToWorld(position);

                findNodes(worldPosition, nodes);

                if (!nodes.empty()) return nodes.front();
            }

            return nullptr;
        }

        std::vector<Node*> Layer::pickNodes(const Vector2& position) const
        {
            std::vector<Node*> result;

            for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
            {
                Camera* camera = *i;

                Vector2 worldPosition = camera->convertNormalizedToWorld(position);

                std::vector<Node*> nodes;
                findNodes(worldPosition, nodes);

                result.insert(result.end(), nodes.begin(), nodes.end());
            }

            return result;
        }

        std::vector<Node*> Layer::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::vector<Node*> result;

            for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
            {
                Camera* camera = *i;

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
    } // namespace scene
} // namespace ouzel
