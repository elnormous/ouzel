// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cassert>
#include <algorithm>
#include "Actor.hpp"
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
                component->setActor(nullptr);
        }

        void Actor::visit(std::vector<Actor*>& drawQueue,
                          const Matrix4<float>& newParentTransform,
                          bool parentTransformDirty,
                          Camera* camera,
                          int32_t parentOrder,
                          bool parentHidden)
        {
            worldOrder = parentOrder + order;
            worldHidden = parentHidden || hidden;

            if (parentTransformDirty) updateTransform(newParentTransform);
            if (transformDirty) calculateTransform();

            if (!worldHidden)
            {
                Box3<float> boundingBox = getBoundingBox();

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
                actor->visit(drawQueue, transform, updateChildrenTransform, camera, worldOrder, worldHidden);

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

        void Actor::setPosition(const Vector2<float>& newPosition)
        {
            position.v[0] = newPosition.v[0];
            position.v[1] = newPosition.v[1];

            updateLocalTransform();
        }

        void Actor::setPosition(const Vector3<float>& newPosition)
        {
            position = newPosition;

            updateLocalTransform();
        }

        void Actor::setRotation(const Quaternion<float>& newRotation)
        {
            rotation = newRotation;

            updateLocalTransform();
        }

        void Actor::setRotation(const Vector3<float>& newRotation)
        {
            Quaternion<float> roationQuaternion;
            roationQuaternion.setEulerAngles(newRotation);

            rotation = roationQuaternion;

            updateLocalTransform();
        }

        void Actor::setRotation(float newRotation)
        {
            Quaternion<float> roationQuaternion;
            roationQuaternion.rotate(newRotation, Vector3<float>(0.0F, 0.0F, 1.0F));

            rotation = roationQuaternion;

            updateLocalTransform();
        }

        void Actor::setScale(const Vector2<float>& newScale)
        {
            scale.v[0] = newScale.v[0];
            scale.v[1] = newScale.v[1];

            updateLocalTransform();
        }

        void Actor::setScale(const Vector3<float>& newScale)
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

        bool Actor::pointOn(const Vector2<float>& worldPosition) const
        {
            Vector2<float> localPosition = Vector2<float>(convertWorldToLocal(Vector3<float>(worldPosition)));

            for (Component* component : components)
            {
                if (component->pointOn(localPosition))
                    return true;
            }

            return false;
        }

        bool Actor::shapeOverlaps(const std::vector<Vector2<float>>& edges) const
        {
            Matrix4<float> inverse = getInverseTransform();

            std::vector<Vector2<float>> transformedEdges;

            for (const Vector2<float>& edge : edges)
            {
                Vector3<float> transformedEdge = Vector3<float>(edge);

                inverse.transformPoint(transformedEdge);

                transformedEdges.push_back(Vector2<float>(transformedEdge.v[0], transformedEdge.v[1]));
            }

            for (Component* component : components)
            {
                if (component->shapeOverlaps(transformedEdges))
                    return true;
            }

            return false;
        }

        void Actor::updateLocalTransform()
        {
            localTransformDirty = transformDirty = inverseTransformDirty = true;
            for (Component* component : components)
                component->updateTransform();
        }

        void Actor::updateTransform(const Matrix4<float>& newParentTransform)
        {
            parentTransform = newParentTransform;
            transformDirty = inverseTransformDirty = true;
            for (Component* component : components)
                component->updateTransform();
        }

        Vector3<float> Actor::getWorldPosition() const
        {
            Vector3<float> result = position;
            getTransform().transformPoint(result);

            return position;
        }

        Vector3<float> Actor::convertWorldToLocal(const Vector3<float>& worldPosition) const
        {
            Vector3<float> localPosition = worldPosition;

            const Matrix4<float>& currentInverseTransform = getInverseTransform();
            currentInverseTransform.transformPoint(localPosition);

            return localPosition;
        }

        Vector3<float> Actor::convertLocalToWorld(const Vector3<float>& localPosition) const
        {
            Vector3<float> worldPosition = localPosition;

            const Matrix4<float>& currentTransform = getTransform();
            currentTransform.transformPoint(worldPosition);

            return worldPosition;
        }

        void Actor::calculateLocalTransform() const
        {
            localTransform.setIdentity();
            localTransform.translate(position);

            Matrix4<float> rotationMatrix;
            rotationMatrix.setRotation(rotation);

            localTransform *= rotationMatrix;

            Vector3<float> finalScale = Vector3<float>(scale.v[0] * (flipX ? -1.0F : 1.0F),
                                                       scale.v[1] * (flipY ? -1.0F : 1.0F),
                                                       scale.v[2]);

            localTransform.scale(finalScale);

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

            auto componentIterator = std::find(components.begin(), components.end(), component);

            if (componentIterator != components.end())
            {
                component->setActor(nullptr);
                components.erase(componentIterator);
                result = true;
            }

            return result;
        }

        void Actor::removeAllComponents()
        {
            for (auto& component : components)
                component->actor = nullptr;

            components.clear();
        }

        void Actor::setLayer(Layer* newLayer)
        {
            ActorContainer::setLayer(newLayer);

            for (Component* component : components)
                component->setLayer(newLayer);
        }

        std::vector<Component*> Actor::getComponents(uint32_t cls) const
        {
            std::vector<Component*> result;

            for (Component* component : components)
            {
                if (component->getClass() == cls)
                    result.push_back(component);
            }

            return result;
        }

        Box3<float> Actor::getBoundingBox() const
        {
            Box3<float> boundingBox;

            for (Component* component : components)
            {
                if (!component->isHidden())
                    boundingBox.merge(component->getBoundingBox());
            }

            return boundingBox;
        }
    } // namespace scene
} // namespace ouzel
