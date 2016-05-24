// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "Types.h"
#include "NodeContainer.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "Rectangle.h"
#include "AABB2.h"
#include "Color.h"

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

            virtual void visit(const Matrix4& newParentTransform, bool parentTransformDirty);
            virtual void process();
            virtual void draw();

            virtual bool addChild(const NodePtr& node) override;
            virtual bool hasParent() const { return !parent.expired(); }
            virtual NodeContainerPtr getParent() const { return parent.lock(); }
            virtual bool removeFromParent();

            virtual void setZ(float newZ);
            virtual float getZ() const { return z; }

            virtual void setGlobalOrder(bool newGlobalOrder);
            virtual bool isGlobalOrder() const { return globalOrder; }

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

            virtual void setVisible(bool newVisible);
            virtual bool isVisible() const { return visible; }

            virtual bool pointOn(const Vector2& worldPosition) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            virtual const Matrix4& getTransform() const;
            const Matrix4& getInverseTransform() const;

            virtual void updateTransform(const Matrix4& newParentTransform);

            Vector2 convertWorldToLocal(const Vector2& worldPosition) const;
            Vector2 convertLocalToWorld(const Vector2& localPosition) const;

            virtual bool checkVisibility() const;

            virtual void animate(const AnimatorPtr& animator);
            virtual AnimatorPtr getAnimator() const { return currentAnimator; }
            virtual void stopAnimation();
            virtual void removeAnimation();

            void setReceiveInput(bool newReceiveInput) { receiveInput = newReceiveInput; }
            bool isReceivingInput() const { return receiveInput; }

            const std::vector<DrawablePtr>& getDrawables() const { return drawables; }
            void addDrawable(DrawablePtr drawable);
            void removeDrawable(uint32_t index);
            void removeDrawable(DrawablePtr drawable);
            void removeAllDrawables();

            LayerPtr getLayer() const { return layer.lock(); }

        protected:
            virtual void addToLayer(const LayerWeakPtr& newLayer);
            virtual void removeFromLayer();

            virtual void calculateLocalTransform() const;
            virtual void calculateTransform() const;

            virtual void calculateInverseTransform() const;

            Matrix4 parentTransform = Matrix4::IDENTITY;
            mutable Matrix4 transform;
            mutable bool transformDirty = true;
            mutable Matrix4 inverseTransform;
            mutable bool inverseTransformDirty = true;

            mutable Matrix4 localTransform;
            mutable bool localTransformDirty = true;

            mutable bool updateChildrenTransform = true;

            //TODO: transform to parent and transform to parent

            Vector2 position;
            float rotation = 0.0f;
            Vector2 scale = Vector2(1.0f, 1.0f);
            graphics::Color color = graphics::Color(255, 255, 255, 255);
            float opacity = 1.0f;
            float z = 0.0f;
            bool globalOrder = true;

            bool flipX = false;
            bool flipY = false;

            bool pickable = true;
            bool visible = true;
            bool receiveInput = false;

            NodeContainerWeakPtr parent;
            LayerWeakPtr layer;

            AnimatorPtr currentAnimator;
            std::vector<DrawablePtr> drawables;
        };
    } // namespace scene
} // namespace ouzel
