// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Node.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Layer.h"
#include "Animator.h"
#include "Camera.h"
#include "Utils.h"
#include "MathUtils.h"
#include "Drawable.h"

namespace ouzel
{
    namespace scene
    {
        Node::Node()
        {

        }

        Node::~Node()
        {

        }

        void Node::visit(const Matrix4& parentTransform, bool parentTransformDirty)
        {
            if (parentTransformDirty)
            {
                updateTransform(parentTransform);
            }

            if (visible)
            {
                if (transformDirty)
                {
                    calculateTransform();
                }

                LayerPtr currentLayer = layer.lock();
                // check if parent is layer
                bool isRoot = !parent.owner_before(layer) && !layer.owner_before(parent);

                if (children.empty())
                {
                    if (currentLayer && (globalOrder || isRoot) && checkVisibility())
                    {
                        currentLayer->addToDrawQueue(std::static_pointer_cast<Node>(shared_from_this()));
                    }
                }
                else
                {
                    lock();

                    std::stable_sort(children.begin(), children.end(), [](const NodePtr& a, const NodePtr& b) {
                        return a->getZ() > b->getZ();
                    });

                    auto i = children.begin();
                    NodePtr node;

                    for (; i != children.end(); ++i)
                    {
                        node = *i;

                        if (!node->remove)
                        {
                            if (node->getZ() < 0.0f)
                            {
                                node->visit(transform, updateChildrenTransform);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    if (currentLayer && (globalOrder || isRoot) && checkVisibility())
                    {
                        currentLayer->addToDrawQueue(std::static_pointer_cast<Node>(shared_from_this()));
                    }

                    for (; i != children.end(); ++i)
                    {
                        if (!node->remove)
                        {
                            node = *i;
                            node->visit(transform, updateChildrenTransform);
                        }
                    }

                    unlock();
                }

                updateChildrenTransform = false;
            }
        }

        void Node::process()
        {
            if (children.empty())
            {
                draw();
            }
            else
            {
                lock();

                auto i = children.begin();
                NodePtr node;

                for (; i != children.end(); ++i)
                {
                    node = *i;

                    if (node->getZ() < 0.0f)
                    {
                        if (!node->isGlobalOrder() && node->isVisible() && node->checkVisibility())
                        {
                            node->draw();
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                draw();

                for (; i != children.end(); ++i)
                {
                    node = *i;

                    if (!node->isGlobalOrder() && node->isVisible() && node->checkVisibility())
                    {
                        node->draw();
                    }
                }

                unlock();
            }
        }

        void Node::draw()
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            if (LayerPtr currentLayer = layer.lock())
            {
                if (currentLayer->getCamera())
                {
                    graphics::Color drawColor = color;
                    drawColor.a = static_cast<uint8_t>(color.a * opacity);

                    for (const DrawablePtr& drawable : drawables)
                    {
                        if (drawable->isVisible())
                        {
                            drawable->draw(currentLayer->getCamera()->getViewProjection(), transform, color);
                        }
                    }
                }
            }
        }

        bool Node::addChild(const NodePtr& node)
        {
            if (NodeContainer::addChild(node))
            {
                node->addToLayer(layer);
                node->updateTransform(getTransform());

                return true;
            }
            else
            {
                return false;
            }
        }

        bool Node::removeFromParent()
        {
            if (NodeContainerPtr currentParent = parent.lock())
            {
                currentParent->removeChild(std::static_pointer_cast<Node>(shared_from_this()));
                return true;
            }

            return false;
        }

        void Node::setZ(float newZ)
        {
            z = newZ;

            // Currently z does not affect transformation
            //localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setGlobalOrder(bool newGlobalOrder)
        {
            globalOrder = newGlobalOrder;
        }

        void Node::setPosition(const Vector2& newPosition)
        {
            position = newPosition;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setRotation(float newRotation)
        {
            rotation = newRotation;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setScale(const Vector2& newScale)
        {
            scale = newScale;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setColor(const graphics::Color& newColor)
        {
            color = newColor;
        }

        void Node::setOpacity(float newOpacity)
        {
            opacity = clamp(newOpacity, 0.0f, 1.0f);
        }

        void Node::setFlipX(bool newFlipX)
        {
            flipX = newFlipX;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setFlipY(bool newFlipY)
        {
            flipY = newFlipY;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setVisible(bool newVisible)
        {
            visible = newVisible;
        }

        void Node::addToLayer(const LayerWeakPtr& newLayer)
        {
            layer = newLayer;

            if (!layer.expired())
            {
                for (const NodePtr& child : children)
                {
                    child->addToLayer(layer);
                }
            }
        }

        void Node::removeFromLayer()
        {
            for (const NodePtr& child : children)
            {
                child->removeFromLayer();
            }

            layer.reset();
        }

        bool Node::pointOn(const Vector2& worldPosition) const
        {
            Vector2 localPosition = convertWorldToLocal(worldPosition);

            for (const DrawablePtr& drawable : drawables)
            {
                if (drawable->pointOn(localPosition))
                {
                    return true;
                }
            }

            return false;
        }

        bool Node::shapeOverlaps(const std::vector<Vector2>& edges) const
        {
            Matrix4 inverse = getInverseTransform();

            std::vector<Vector2> transformedEdges;

            for (const Vector2& edge : edges)
            {
                Vector3 transformedEdge = edge;

                inverse.transformPoint(transformedEdge);

                transformedEdges.push_back(Vector2(transformedEdge.x, transformedEdge.y));
            }

            for (const DrawablePtr& drawable : drawables)
            {
                if (drawable->shapeOverlaps(transformedEdges))
                {
                    return true;
                }
            }

            return false;
        }

        const Matrix4& Node::getTransform() const
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            return transform;
        }

        const Matrix4& Node::getInverseTransform() const
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            if (inverseTransformDirty)
            {
                calculateInverseTransform();
            }

            return inverseTransform;
        }

        void Node::updateTransform(const Matrix4& newParentTransform)
        {
            parentTransform = newParentTransform;
            transformDirty = inverseTransformDirty = true;
        }

        Vector2 Node::convertWorldToLocal(const Vector2& worldPosition) const
        {
            Vector3 localPosition = worldPosition;

            const Matrix4& inverseTransform = getInverseTransform();
            inverseTransform.transformPoint(localPosition);

            return Vector2(localPosition.x, localPosition.y);
        }

        Vector2 Node::convertLocalToWorld(const Vector2& localPosition) const
        {
            Vector3 worldPosition = localPosition;

            const Matrix4& transform = getTransform();
            transform.transformPoint(worldPosition);

            return Vector2(worldPosition.x, worldPosition.y);
        }

        bool Node::checkVisibility() const
        {
            if (LayerPtr currentLayer = layer.lock())
            {
                for (const DrawablePtr& drawable : drawables)
                {
                    if (drawable->isVisible() &&
                        (drawable->getBoundingBox().isEmpty() ||
                         sharedEngine->getRenderer()->checkVisibility(getTransform(), drawable->getBoundingBox(), currentLayer->getCamera())))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        void Node::animate(const AnimatorPtr& animator)
        {
            stopAnimation();
            currentAnimator = animator;

            if (currentAnimator)
            {
                currentAnimator->start(std::static_pointer_cast<Node>(shared_from_this()));
            }
        }

        void Node::stopAnimation()
        {
            if (currentAnimator)
            {
                currentAnimator->stop();
                removeAnimation();
            }
        }

        void Node::removeAnimation()
        {
            currentAnimator.reset();
        }

        void Node::calculateLocalTransform() const
        {
            localTransform = Matrix4::IDENTITY;
            localTransform.translate(Vector3(position.x, position.y, 0.0f));
            localTransform.rotateZ(TAU - rotation);

            Vector3 realScale = Vector3(scale.x * (flipX ? -1.0f : 1.0f),
                                        scale.y * (flipY ? -1.0f : 1.0f),
                                        1.0f);

            localTransform.scale(realScale);

            localTransformDirty = false;
        }

        void Node::calculateTransform() const
        {
            if (localTransformDirty)
            {
                calculateLocalTransform();
            }

            transform = parentTransform * localTransform;
            transformDirty = false;

            updateChildrenTransform = true;
        }

        void Node::calculateInverseTransform() const
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            inverseTransform = transform;
            inverseTransform.invert();
            inverseTransformDirty = false;
        }

        void Node::addDrawable(DrawablePtr drawable)
        {
            drawables.push_back(drawable);
            drawable->setParentNode(std::static_pointer_cast<Node>(shared_from_this()));
        }

        void Node::removeDrawable(uint32_t index)
        {
            if (index >= drawables.size())
            {
                return;
            }

            drawables.erase(drawables.begin() + index);
        }

        void Node::removeDrawable(DrawablePtr drawable)
        {
            for (auto i = drawables.begin(); i != drawables.end();)
            {
                if (*i == drawable)
                {
                    i = drawables.erase(i);
                }
                else
                {
                    ++i;
                }
            }
        }

        void Node::removeAllDrawables()
        {
            drawables.clear();
        }

    } // namespace scene
} // namespace ouzel
