// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "utils/Types.h"
#include "scene/NodeContainer.h"
#include "math/Vector2.h"
#include "math/Matrix4.h"
#include "math/Rectangle.h"
#include "math/AABB2.h"
#include "graphics/Color.h"
#include "core/UpdateCallback.h"

namespace ouzel
{
    namespace scene
    {
        class Node: public NodeContainer
        {
            friend NodeContainer;
            friend Layer;
            friend Animator;
        public:
            Node();
            virtual ~Node();

            virtual void addChild(Node* node) override;
            virtual NodeContainer* getParent() const { return parent; }
            virtual void removeFromParent();

            virtual void setName(const std::string& newName) { name = newName; }
            virtual const std::string& getName() const { return name; }

            virtual void setZ(float newZ);
            virtual float getZ() const { return z; }

            virtual void setPosition(const Vector2& newPosition);
            virtual const Vector2& getPosition() const { return position; }

            virtual void setRotation(float newRotation);
            virtual float getRotation() const { return rotation; }

            virtual void setScale(const Vector2& newScale);
            virtual const Vector2& getScale() const { return scale; }

            virtual void setColor(const graphics::Color& newColor);
            virtual const graphics::Color& getColor() const { return color; }

            virtual void setOpacity(float newOpacity);
            virtual float getOpacity() const { return opacity; }

            virtual void setFlipX(bool newFlipX);
            virtual bool getFlipX() const { return flipX; }

            virtual void setFlipY(bool newFlipY);
            virtual bool getFlipY() const { return flipY; }

            virtual void setPickable(bool newPickable) { pickable = newPickable; }
            virtual bool isPickable() const { return pickable; }

            bool isCullDisabled() const { return cullDisabled; }
            void setCullDisabled(bool newCullDisabled) { cullDisabled = newCullDisabled; }

            virtual void setHidden(bool newHidden);
            virtual bool isHidden() const { return hidden; }

            virtual bool pointOn(const Vector2& worldPosition) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

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

            virtual void updateTransform(const Matrix4& newParentTransform);

            Vector2 getWorldPosition() const;
            float getWorldZ() const { return parentZ + z; }

            Vector2 convertWorldToLocal(const Vector2& worldPosition) const;
            Vector2 convertLocalToWorld(const Vector2& localPosition) const;

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

            virtual void visit(std::vector<Node*>& drawQueue,
                               const Matrix4& newParentTransform,
                               bool parentTransformDirty,
                               Camera* camera,
                               float newParentZ);
            virtual void draw(Camera* camera);
            virtual void drawWireframe(Camera* camera);

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

            std::string name;

            Vector2 position;
            float rotation = 0.0f;
            Vector2 scale = Vector2(1.0f, 1.0f);
            graphics::Color color = graphics::Color::WHITE;
            float opacity = 1.0f;
            float parentZ = 0.0f;
            float z = 0.0f;

            Animator* currentAnimator = nullptr;
            std::vector<Component*> components;

            NodeContainer* parent = nullptr;

            UpdateCallback animationUpdateCallback;
        };
    } // namespace scene
} // namespace ouzel
