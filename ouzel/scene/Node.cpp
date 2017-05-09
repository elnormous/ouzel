// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Node.h"
#include "core/Engine.h"
#include "SceneManager.h"
#include "Layer.h"
#include "Camera.h"
#include "math/MathUtils.h"
#include "Component.h"

namespace ouzel
{
    namespace scene
    {
        Node::Node()
        {
        }

        Node::~Node()
        {
            if (parent) parent->removeChild(this);

            for (const auto& component : components)
            {
                component->node = nullptr;
            }
        }

        void Node::visit(std::vector<Node*>& drawQueue,
                         const Matrix4& newParentTransform,
                         bool parentTransformDirty,
                         Camera* camera,
                         int32_t parentOrder,
                         bool parentHidden)
        {
            worldOrder = parentOrder + order;
            worldHidden = parentHidden || hidden;

            if (parentTransformDirty)
            {
                updateTransform(newParentTransform);
            }

            if (transformDirty)
            {
                calculateTransform();
            }

            if (!worldHidden)
            {
                Box3 boundingBox = getBoundingBox();

                if (cullDisabled || (!boundingBox.isEmpty() && camera->checkVisibility(getTransform(), boundingBox)))
                {
                    auto upperBound = std::upper_bound(drawQueue.begin(), drawQueue.end(), this,
                                                       [](Node* a, Node* b) {
                                                           return a->worldOrder > b->worldOrder;
                                                       });

                    drawQueue.insert(upperBound, this);
                }
            }

            for (Node* node : children)
            {
                node->visit(drawQueue, transform, updateChildrenTransform, camera, worldOrder, worldHidden);
            }

            updateChildrenTransform = false;
        }

        void Node::draw(Camera* camera, bool wireframe)
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            Color drawColor(color.v[0], color.v[1], color.v[2],
                            wireframe ? 255 : (static_cast<uint8_t>(color.v[3] * opacity)));

            for (Component* component : components)
            {
                if (!component->isHidden())
                {
                    component->draw(transform,
                                    drawColor,
                                    camera->getRenderViewProjection(),
                                    camera->getRenderTarget(),
                                    camera->getRenderViewport(),
                                    camera->getDepthWrite(),
                                    camera->getDepthTest(),
                                    wireframe,
                                    false,
                                    Rectangle());
                }
            }
        }

        void Node::addChild(Node* node)
        {
            NodeContainer::addChild(node);

            if (node)
            {
                node->updateTransform(getTransform());
            }
        }

        void Node::setPosition(const Vector2& newPosition)
        {
            position.v[0] = newPosition.v[0];
            position.v[1] = newPosition.v[1];

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setPosition(const Vector3& newPosition)
        {
            position = newPosition;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setRotation(const Quaternion& newRotation)
        {
            rotation = newRotation;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setRotation(const Vector3& newRotation)
        {
            Quaternion roationQuaternion;
            roationQuaternion.setEulerAngles(newRotation);

            rotation = roationQuaternion;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setRotation(float newRotation)
        {
            Quaternion roationQuaternion;
            roationQuaternion.rotate(newRotation, Vector3(0.0f, 0.0f, 1.0f));

            rotation = roationQuaternion;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setScale(const Vector2& newScale)
        {
            scale.v[0] = newScale.v[0];
            scale.v[1] = newScale.v[1];

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setScale(const Vector3& newScale)
        {
            scale = newScale;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setColor(const Color& newColor)
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

        void Node::setHidden(bool newHidden)
        {
            hidden = newHidden;
        }

        bool Node::pointOn(const Vector2& worldPosition) const
        {
            Vector2 localPosition = convertWorldToLocal(worldPosition);

            for (Component* component : components)
            {
                if (component->pointOn(localPosition))
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

                transformedEdges.push_back(Vector2(transformedEdge.v[0], transformedEdge.v[1]));
            }

            for (Component* component : components)
            {
                if (component->shapeOverlaps(transformedEdges))
                {
                    return true;
                }
            }

            return false;
        }

        void Node::updateTransform(const Matrix4& newParentTransform)
        {
            parentTransform = newParentTransform;
            transformDirty = inverseTransformDirty = true;
        }

        Vector3 Node::getWorldPosition() const
        {
            Vector3 result = position;
            getTransform().transformPoint(result);

            return position;
        }

        Vector3 Node::convertWorldToLocal(const Vector3& worldPosition) const
        {
            Vector3 localPosition = worldPosition;

            const Matrix4& currentInverseTransform = getInverseTransform();
            currentInverseTransform.transformPoint(localPosition);

            return localPosition;
        }

        Vector3 Node::convertLocalToWorld(const Vector3& localPosition) const
        {
            Vector3 worldPosition = localPosition;

            const Matrix4& currentTransform = getTransform();
            currentTransform.transformPoint(worldPosition);

            return worldPosition;
        }

        void Node::calculateLocalTransform() const
        {
            localTransform.setIdentity();
            localTransform.translate(position);
            localTransform *= rotation.getMatrix();

            Vector3 realScale = Vector3(scale.v[0] * (flipX ? -1.0f : 1.0f),
                                        scale.v[1] * (flipY ? -1.0f : 1.0f),
                                        scale.v[2]);

            localTransform.scale(realScale);

            localTransformDirty = false;
        }

        void Node::calculateTransform() const
        {
            transform = parentTransform * getLocalTransform();
            transformDirty = false;

            updateChildrenTransform = true;
        }

        void Node::calculateInverseTransform() const
        {
            inverseTransform = getTransform();
            inverseTransform.invert();
            inverseTransformDirty = false;
        }

        void Node::addComponent(Component* component)
        {
            if (component->node)
            {
                component->node->removeComponent(component);
            }

            component->node = this;
            components.push_back(component);
        }

        void Node::addComponent(std::unique_ptr<Component>&& component)
        {
            addComponent(component.get());
            ownedComponents.push_back(std::forward<std::unique_ptr<Component>>(component));
        }

        bool Node::removeComponent(uint32_t index)
        {
            if (index >= components.size())
            {
                return false;
            }

            return removeComponent(components[index]);
        }

        bool Node::removeComponent(Component* component)
        {
            std::vector<std::unique_ptr<Component>>::iterator ownedIterator = std::find_if(ownedComponents.begin(), ownedComponents.end(), [component](const std::unique_ptr<Component>& other) {
                return other.get() == component;
            });

            if (ownedIterator != ownedComponents.end())
            {
                ownedComponents.erase(ownedIterator);
            }

            std::vector<Component*>::iterator componentIterator = std::find(components.begin(), components.end(), component);

            if (componentIterator != components.end())
            {
                component->node = nullptr;
                components.erase(componentIterator);
                return true;
            }

            return false;
        }

        void Node::removeAllComponents()
        {
            components.clear();
        }

        Box3 Node::getBoundingBox() const
        {
            Box3 boundingBox;

            for (Component* component : components)
            {
                if (!component->isHidden())
                {
                    boundingBox.merge(component->getBoundingBox());
                }
            }

            return boundingBox;
        }
    } // namespace scene
} // namespace ouzel
