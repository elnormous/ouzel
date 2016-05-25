// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Engine.h"
#include "Layer.h"
#include "Node.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4.h"
#include "Drawable.h"

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
                sharedEngine->getRenderer()->activateRenderTarget(renderTarget);

                auto childrenCopy = children;

                for (const NodePtr& child : childrenCopy)
                {
                    if (child->isVisible())
                    {
                        if (checkVisibility(child))
                        {
                            addToDrawQueue(child);
                        }

                        child->visit(Matrix4::IDENTITY, false, std::static_pointer_cast<Layer>(shared_from_this()));
                    }
                }

                std::stable_sort(drawQueue.begin(), drawQueue.end(), [](const NodePtr& a, const NodePtr& b) {
                    return a->getZ() > b->getZ();
                });

                for (const NodePtr& node : drawQueue)
                {
                    node->process(std::static_pointer_cast<Layer>(shared_from_this()));
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

        void Layer::addToDrawQueue(const NodePtr& node)
        {
            drawQueue.push_back(node);
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
            for (std::vector<NodePtr>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
            {
                NodePtr node = *i;

                if (node->isVisible() && node->isPickable() && node->pointOn(position))
                {
                    return node;
                }
            }

            return nullptr;
        }

        std::set<NodePtr> Layer::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::set<NodePtr> result;

            for (std::vector<NodePtr>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
            {
                NodePtr node = *i;

                if (node->isVisible() && node->isPickable() && node->shapeOverlaps(edges))
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
                // we must add this node to draw queue if it has children
                if (!node->getChildren().empty())
                {
                    return true;
                }

                for (const DrawablePtr& drawable : node->getDrawables())
                {
                    if (drawable->isVisible() &&
                        (drawable->getBoundingBox().isEmpty() ||
                         sharedEngine->getRenderer()->checkVisibility(node->getTransform(), drawable->getBoundingBox(), camera)))
                    {
                        return true;
                    }
                }
            }

            return false;
        }
    } // namespace scene
} // namespace ouzel
