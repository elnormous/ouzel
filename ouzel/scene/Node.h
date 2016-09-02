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

namespace ouzel
{
    namespace scene
    {
        class SceneManager;

        class Node: public NodeContainer
        {
            friend SceneManager;
            friend NodeContainer;
            friend Layer;
        public:
            Node();
            virtual ~Node();

            virtual bool addChild(const NodePtr& node) override;
            virtual bool hasParent() const { return (parent != nullptr); }

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

            virtual void setHidden(bool newHidden);
            virtual bool isHidden() const { return hidden; }

            virtual bool pointOn(const Vector2& worldPosition) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            virtual const Matrix4& getTransform() const;
            const Matrix4& getInverseTransform() const;

            virtual void updateTransform(const Matrix4& newParentTransform);

            Vector2 convertWorldToLocal(const Vector2& worldPosition) const;
            Vector2 convertLocalToWorld(const Vector2& localPosition) const;

            virtual void animate(const AnimatorPtr& animator);
            virtual AnimatorPtr getAnimator() const { return currentAnimator; }
            virtual void stopAnimation();
            virtual void removeAnimation();

            void setReceiveInput(bool newReceiveInput) { receiveInput = newReceiveInput; }
            bool isReceivingInput() const { return receiveInput; }

            const std::vector<ComponentPtr>& getComponents() const { return components; }
            bool addComponent(const ComponentPtr& component);
            bool removeComponent(uint32_t index);
            bool removeComponent(const ComponentPtr& component);
            void removeAllComponents();

        protected:
            void setParent(NodeContainer* newParent) { parent = newParent; }

            virtual void visit(const Matrix4& newParentTransform, bool parentTransformDirty, const LayerPtr& currentLayer, float depth);
            virtual void draw(const LayerPtr& currentLayer);
            virtual void drawWireframe(const LayerPtr& currentLayer);

            virtual void calculateLocalTransform() const;
            virtual void calculateTransform() const;

            virtual void calculateInverseTransform() const;

            Matrix4 parentTransform = Matrix4::IDENTITY;
            mutable Matrix4 transform;
            mutable Matrix4 inverseTransform;
            mutable Matrix4 localTransform;

            mutable bool transformDirty = true;
            mutable bool inverseTransformDirty = true;
            mutable bool localTransformDirty = true;
            mutable bool updateChildrenTransform = true;

            bool flipX = false;
            bool flipY = false;

            bool pickable = true;
            bool hidden = false;
            bool receiveInput = false;

            std::string name;

            Vector2 position;
            float rotation = 0.0f;
            Vector2 scale = Vector2(1.0f, 1.0f);
            graphics::Color color = graphics::Color(255, 255, 255, 255);
            float opacity = 1.0f;
            float z = 0.0f;

            AnimatorPtr currentAnimator;
            std::vector<ComponentPtr> components;

            NodeContainer* parent = nullptr;
        };
    } // namespace scene
} // namespace ouzel
