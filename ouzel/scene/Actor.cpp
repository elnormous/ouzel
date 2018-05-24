// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>
#include <algorithm>
#include "Actor.hpp"
#include "core/Engine.hpp"
#include "SceneManager.hpp"
#include "Layer.hpp"
#include "Camera.hpp"
#include "math/MathUtils.hpp"
#include "Component.hpp"

namespace ouzel
{
    namespace scene
    {
        Actor::Actor()
        {
        }

        Actor::~Actor()
        {
            if (parent) parent->removeChild(this);

            for (const auto& component : components)
            {
                component->setActor(nullptr);
            }
        }

        void Actor::visit(std::vector<Actor*>& drawQueue,
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
                                                       [](Actor* a, Actor* b) {
                                                           return a->worldOrder > b->worldOrder;
                                                       });

                    drawQueue.insert(upperBound, this);
                }
            }

            for (Actor* actor : children)
            {
                actor->visit(drawQueue, transform, updateChildrenTransform, camera, worldOrder, worldHidden);
            }

            updateChildrenTransform = false;
        }

        void Actor::draw(Camera* camera, bool wireframe)
        {
            if (transformDirty)
                calculateTransform();

            for (Component* component : components)
            {
                if (!component->isHidden())
                {
                    component->draw(transform,
                                    opacity,
                                    camera->getRenderViewProjection(),
                                    wireframe);
                }
            }
        }

        void Actor::addChild(Actor* actor)
        {
            ActorContainer::addChild(actor);

            actor->updateTransform(getTransform());
        }

        void Actor::setPosition(const Vector2& newPosition)
        {
            position.x = newPosition.x;
            position.y = newPosition.y;

            updateLocalTransform();
        }

        void Actor::setPosition(const Vector3& newPosition)
        {
            position = newPosition;

            updateLocalTransform();
        }

        void Actor::setRotation(const Quaternion& newRotation)
        {
            rotation = newRotation;

            updateLocalTransform();
        }

        void Actor::setRotation(const Vector3& newRotation)
        {
            Quaternion roationQuaternion;
            roationQuaternion.setEulerAngles(newRotation);

            rotation = roationQuaternion;

            updateLocalTransform();
        }

        void Actor::setRotation(float newRotation)
        {
            Quaternion roationQuaternion;
            roationQuaternion.rotate(newRotation, Vector3(0.0F, 0.0F, 1.0F));

            rotation = roationQuaternion;

            updateLocalTransform();
        }

        void Actor::setScale(const Vector2& newScale)
        {
            scale.x = newScale.x;
            scale.y = newScale.y;

            updateLocalTransform();
        }

        void Actor::setScale(const Vector3& newScale)
        {
            scale = newScale;

            updateLocalTransform();
        }

        void Actor::setOpacity(float newOpacity)
        {
            opacity = clamp(newOpacity, 0.0F, 1.0F);
        }

        void Actor::setFlipX(bool newFlipX)
        {
            flipX = newFlipX;

            updateLocalTransform();
        }

        void Actor::setFlipY(bool newFlipY)
        {
            flipY = newFlipY;

            updateLocalTransform();
        }

        void Actor::setHidden(bool newHidden)
        {
            hidden = newHidden;
        }

        bool Actor::pointOn(const Vector2& worldPosition) const
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

        bool Actor::shapeOverlaps(const std::vector<Vector2>& edges) const
        {
            Matrix4 inverse = getInverseTransform();

            std::vector<Vector2> transformedEdges;

            for (const Vector2& edge : edges)
            {
                Vector3 transformedEdge = edge;

                inverse.transformPoint(transformedEdge);

                transformedEdges.push_back(Vector2(transformedEdge.x, transformedEdge.y));
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

        void Actor::updateLocalTransform()
        {
            localTransformDirty = transformDirty = inverseTransformDirty = true;
            for (Component* component : components)
            {
                component->updateTransform();
            }
        }

        void Actor::updateTransform(const Matrix4& newParentTransform)
        {
            parentTransform = newParentTransform;
            transformDirty = inverseTransformDirty = true;
            for (Component* component : components)
            {
                component->updateTransform();
            }
        }

        Vector3 Actor::getWorldPosition() const
        {
            Vector3 result = position;
            getTransform().transformPoint(result);

            return position;
        }

        Vector3 Actor::convertWorldToLocal(const Vector3& worldPosition) const
        {
            Vector3 localPosition = worldPosition;

            const Matrix4& currentInverseTransform = getInverseTransform();
            currentInverseTransform.transformPoint(localPosition);

            return localPosition;
        }

        Vector3 Actor::convertLocalToWorld(const Vector3& localPosition) const
        {
            Vector3 worldPosition = localPosition;

            const Matrix4& currentTransform = getTransform();
            currentTransform.transformPoint(worldPosition);

            return worldPosition;
        }

        void Actor::calculateLocalTransform() const
        {
            localTransform.setIdentity();
            localTransform.translate(position);

            Matrix4 rotationMatrix;
            rotationMatrix.setRotation(rotation);

            localTransform *= rotationMatrix;

            Vector3 realScale = Vector3(scale.x * (flipX ? -1.0F : 1.0F),
                                        scale.y * (flipY ? -1.0F : 1.0F),
                                        scale.z);

            localTransform.scale(realScale);

            localTransformDirty = false;
        }

        void Actor::calculateTransform() const
        {
            transform = parentTransform * getLocalTransform();
            transformDirty = false;

            updateChildrenTransform = true;
        }

        void Actor::calculateInverseTransform() const
        {
            inverseTransform = getTransform();
            inverseTransform.invert();
            inverseTransformDirty = false;
        }

        void Actor::removeFromParent()
        {
            if (parent) parent->removeChild(this);
        }

        void Actor::addComponent(Component* component)
        {
            assert(component);

            if (component->actor)
                component->actor->removeComponent(component);

            component->setActor(this);
            components.push_back(component);
        }

        bool Actor::removeComponent(Component* component)
        {
            bool result = false;

            std::vector<Component*>::iterator componentIterator = std::find(components.begin(), components.end(), component);

            if (componentIterator != components.end())
            {
                component->setActor(nullptr);
                components.erase(componentIterator);
                result = true;
            }

            std::vector<std::unique_ptr<Component>>::iterator ownedIterator = std::find_if(ownedComponents.begin(), ownedComponents.end(), [component](const std::unique_ptr<Component>& other) {
                return other.get() == component;
            });

            if (ownedIterator != ownedComponents.end())
            {
                ownedComponents.erase(ownedIterator);
            }

            return result;
        }

        void Actor::removeAllComponents()
        {
            components.clear();
            ownedComponents.clear();
        }

        void Actor::setLayer(Layer* newLayer)
        {
            ActorContainer::setLayer(newLayer);

            for (Component* component : components)
            {
                component->setLayer(newLayer);
            }
        }

        std::vector<Component*> Actor::getComponents(uint32_t cls) const
        {
            std::vector<Component*> result;

            for (Component* component : components)
            {
                if (component->getClass() == cls)
                {
                    result.push_back(component);
                }
            }

            return result;
        }

        Box3 Actor::getBoundingBox() const
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
