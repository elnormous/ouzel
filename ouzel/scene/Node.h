// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "scene/NodeContainer.h"
#include "math/AABB2.h"
#include "math/Color.h"
#include "math/Matrix4.h"
#include "math/Quaternion.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "core/UpdateCallback.h"

namespace ouzel
{
    namespace scene
    {
        class Animator;
        class Camera;
        class Component;
        class Layer;

        class Node: public NodeContainer
        {
            friend NodeContainer;
            friend Layer;
            friend Animator;
        public:
            Node();
            virtual ~Node();

            virtual void visit(std::vector<Node*>& drawQueue,
                               const Matrix4& newParentTransform,
                               bool parentTransformDirty,
                               Camera* camera,
                               int32_t parentOrder);
            virtual void draw(Camera* camera);
            virtual void drawWireframe(Camera* camera);

            virtual void addChild(Node* node) override;
            virtual NodeContainer* getParent() const { return parent; }
            virtual void removeFromParent();

            virtual void setPosition(const Vector2& newPosition);
            virtual void setPosition(const Vector3& newPosition);
            virtual const Vector3& getPosition() const { return position; }

            void setOrder(int32_t newOrder) { order = newOrder; }
            int32_t getOrder() const { return order; }

            virtual void setRotation(const Quaternion& newRotation);
            virtual void setRotation(const Vector3& newRotation);
            virtual void setRotation(float newRotation);
            virtual const Quaternion& getRotation() const { return rotation; }

            virtual void setScale(const Vector2& newScale);
            virtual void setScale(const Vector3& newScale);
            virtual const Vector3& getScale() const { return scale; }

            virtual void setColor(const Color& newColor);
            virtual const Color& getColor() const { return color; }

            virtual void setOpacity(float newOpacity);
            virtual float getOpacity() const { return opacity; }

            virtual void setFlipX(bool newFlipX);
            virtual bool getFlipX() const { return flipX; }

            virtual void setFlipY(bool newFlipY);
            virtual bool getFlipY() const { return flipY; }

            virtual void setPickable(bool newPickable) { pickable = newPickable; }
            virtual bool isPickable() const { return pickable; }

            virtual bool isCullDisabled() const { return cullDisabled; }
            virtual void setCullDisabled(bool newCullDisabled) { cullDisabled = newCullDisabled; }

            virtual void setHidden(bool newHidden);
            virtual bool isHidden() const { return hidden; }

            virtual bool pointOn(const Vector2& worldPosition) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            const Matrix4& getLocalTransform() const
            {
                if (localTransformDirty)
                {
                    calculateLocalTransform();
                }

                return localTransform;
            }

            const Matrix4& getTransform() const
            {
                if (transformDirty)
                {
                    calculateTransform();
                }

                return transform;
            }

            const Matrix4& getInverseTransform() const
            {
                if (inverseTransformDirty)
                {
                    calculateInverseTransform();
                }

                return inverseTransform;
            }

            virtual void updateTransform(const Matrix4& newParentTransform);

            Vector3 getWorldPosition() const;
            virtual int32_t getWorldOrder() const { return worldOrder; }

            Vector3 convertWorldToLocal(const Vector3& worldPosition) const;
            Vector3 convertLocalToWorld(const Vector3& localPosition) const;

            void animate(Animator* animator);
            Animator* getAnimator() const { return currentAnimator; }
            void removeCurrentAnimator();

            const std::vector<Component*>& getComponents() const { return components; }
            void addComponent(Component* component);
            bool removeComponent(uint32_t index);
            bool removeComponent(Component* component);
            void removeAllComponents();

            AABB2 getBoundingBox() const;

        protected:
            void removeAnimator(Animator* animator);

            virtual void calculateLocalTransform() const;
            virtual void calculateTransform() const;

            virtual void calculateInverseTransform() const;

            void updateAnimation(float delta);

            Matrix4 parentTransform;
            mutable Matrix4 transform;
            mutable Matrix4 inverseTransform;
            mutable Matrix4 localTransform;

            mutable bool transformDirty = true;
            mutable bool inverseTransformDirty = true;
            mutable bool localTransformDirty = true;
            mutable bool updateChildrenTransform = true;

            bool flipX = false;
            bool flipY = false;

            bool pickable = false;
            bool cullDisabled = false;
            bool hidden = false;

            Vector3 position;
            Quaternion rotation = Quaternion::IDENTITY;
            Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
            Color color = Color::WHITE;
            float opacity = 1.0f;
            int32_t worldOrder = 0;
            int32_t order = 0;

            Animator* currentAnimator = nullptr;
            std::vector<Component*> components;

            NodeContainer* parent = nullptr;

            UpdateCallback animationUpdateCallback;
        };
    } // namespace scene
} // namespace ouzel
