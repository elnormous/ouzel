// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

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
        }

        Layer::~Layer()
        {
            for (const CameraPtr& camera : cameras)
            {
                camera->removeFromLayer();
            }
        }

        void Layer::draw()
        {
            drawQueue.clear();

            for (const CameraPtr& camera : cameras)
            {
                for (const NodePtr& child : children)
                {
                    if (!child->isHidden())
                    {
                        child->visit(Matrix4::IDENTITY, false, camera, this, 0.0f);
                    }
                }

                drawQueue.sort([](const std::pair<NodePtr, float>& a, const std::pair<NodePtr, float>& b) {
                    return a.second > b.second;
                });

                for (const auto& node : drawQueue)
                {
                    node.first->draw(camera);
                }

                if (wireframe)
                {
                    for (const auto& node : drawQueue)
                    {
                        node.first->drawWireframe(camera);
                    }
                }
            }
        }

        bool Layer::addChild(const NodePtr& node)
        {
            if (NodeContainer::addChild(node))
            {
                node->updateTransform(Matrix4::IDENTITY);

                return true;
            }
            else
            {
                return false;
            }
        }

        void Layer::addToDrawQueue(const NodePtr& node, float depth)
        {
            drawQueue.push_back({ node, depth });
        }

        void Layer::addCamera(const CameraPtr& camera)
        {
            auto i = cameras.insert(camera);

            if (i.second)
            {
                camera->addToLayer(this);
                camera->recalculateProjection();
            }
        }

        void Layer::removeCamera(const CameraPtr& camera)
        {
            auto i = cameras.find(camera);

            if (i != cameras.end())
            {
                camera->removeFromLayer();
                cameras.erase(i);
            }
        }

        NodePtr Layer::pickNode(const Vector2& position) const
        {
            for (const CameraPtr& camera : cameras)
            {
                Vector2 worldPosition = camera->convertNormalizedToWorld(position);

                for (std::list<std::pair<NodePtr, float>>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
                {
                    const NodePtr& node = i->first;

                    if (!node->isHidden() && node->isPickable() && node->pointOn(worldPosition))
                    {
                        return node;
                    }
                }
            }

            return nullptr;
        }

        std::vector<NodePtr> Layer::pickNodes(const Vector2& position) const
        {
            std::vector<NodePtr> result;

            for (const CameraPtr& camera : cameras)
            {
                Vector2 worldPosition = camera->convertNormalizedToWorld(position);

                for (std::list<std::pair<NodePtr, float>>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
                {
                    const NodePtr& node = i->first;

                    if (!node->isHidden() && node->isPickable() && node->pointOn(worldPosition))
                    {
                        result.push_back(node);
                    }
                }
            }

            return result;
        }

        std::set<NodePtr> Layer::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::set<NodePtr> result;

            for (const CameraPtr& camera : cameras)
            {
                std::vector<Vector2> worldEdges;
                worldEdges.reserve(edges.size());

                for (const Vector2& edge : edges)
                {
                    worldEdges.push_back(camera->convertNormalizedToWorld(edge));
                }

                for (std::list<std::pair<NodePtr, float>>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
                {
                    const NodePtr& node = i->first;

                    if (!node->isHidden() && node->isPickable() && node->shapeOverlaps(worldEdges))
                    {
                        result.insert(node);
                    }
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
            for (const CameraPtr& camera : cameras)
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
