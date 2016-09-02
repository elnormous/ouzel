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

        }

        void Layer::draw()
        {
            drawQueue.clear();

            // render only if there is an active camera
            if (camera)
            {
                for (const NodePtr& child : children)
                {
                    if (!child->isHidden())
                    {
                        child->visit(Matrix4::IDENTITY, false, std::static_pointer_cast<Layer>(shared_from_this()), 0.0f);
                    }
                }

                drawQueue.sort([](const std::pair<NodePtr, float>& a, const std::pair<NodePtr, float>& b) {
                    return a.second > b.second;
                });

                for (const auto& node : drawQueue)
                {
                    node.first->draw(std::static_pointer_cast<Layer>(shared_from_this()));
                }

                if (wireframe)
                {
                    for (const auto& node : drawQueue)
                    {
                        node.first->drawWireframe(std::static_pointer_cast<Layer>(shared_from_this()));
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

        void Layer::setCamera(const CameraPtr& newCamera)
        {
            if (camera)
            {
                camera->removeFromLayer();
            }

            camera = newCamera;

            if (camera)
            {
                camera->addToLayer(std::static_pointer_cast<Layer>(shared_from_this()));
                camera->recalculateProjection();
            }
        }

        NodePtr Layer::pickNode(const Vector2& position) const
        {
            for (std::list<std::pair<NodePtr, float>>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
            {
                const NodePtr& node = i->first;

                if (!node->isHidden() && node->isPickable() && node->pointOn(position))
                {
                    return node;
                }
            }

            return nullptr;
        }

        std::vector<NodePtr> Layer::pickNodes(const Vector2& position) const
        {
            std::vector<NodePtr> result;

            for (std::list<std::pair<NodePtr, float>>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
            {
                const NodePtr& node = i->first;

                if (!node->isHidden() && node->isPickable() && node->pointOn(position))
                {
                    result.push_back(node);
                }
            }

            return result;
        }

        std::set<NodePtr> Layer::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::set<NodePtr> result;

            for (std::list<std::pair<NodePtr, float>>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
            {
                const NodePtr& node = i->first;

                if (!node->isHidden() && node->isPickable() && node->shapeOverlaps(edges))
                {
                    result.insert(node);
                }
            }

            return result;
        }

        void Layer::setOrder(int32_t newOrder)
        {
            order = newOrder;
        }

        void Layer::setRenderTarget(const graphics::RenderTargetPtr& newRenderTarget)
        {
            renderTarget = newRenderTarget;

            if (camera)
            {
                camera->recalculateProjection();
            }
        }

        bool Layer::checkVisibility(const NodePtr& node) const
        {
            if (camera)
            {
                AABB2 boundingBox;

                for (const ComponentPtr& component : node->getComponents())
                {
                    if (!component->isHidden())
                    {
                        boundingBox.merge(component->getBoundingBox());
                    }
                }

                if (!boundingBox.isEmpty() && camera->checkVisibility(node->getTransform(), boundingBox))
                {
                    return true;
                }
            }

            return false;
        }
    } // namespace scene
} // namespace ouzel
