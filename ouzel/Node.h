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

            virtual void visit(const Matrix4& parentTransform, bool parentTransformDirty);
            virtual void process();
            virtual void draw();

            virtual bool addChild(const NodePtr& node) override;
            virtual bool hasParent() const { return !_parent.expired(); }
            virtual NodeContainerPtr getParent() const { return _parent.lock(); }
            virtual bool removeFromParent();

            virtual void setZ(float z);
            virtual float getZ() const { return _z; }

            virtual void setGlobalOrder(bool globalOrder);
            virtual bool isGlobalOrder() const { return _globalOrder; }

            virtual void setPosition(const Vector2& position);
            virtual const Vector2& getPosition() const { return _position; }

            virtual void setRotation(float rotation);
            virtual float getRotation() const { return _rotation; }

            virtual void setScale(const Vector2& scale);
            virtual const Vector2& getScale() const { return _scale; }

            virtual void setColor(const graphics::Color& color);
            virtual const graphics::Color& getColor() const { return _color; }

            virtual void setOpacity(float opacity);
            virtual float getOpacity() const { return _opacity; }

            virtual void setFlipX(bool flipX);
            virtual bool getFlipX() const { return _flipX; }

            virtual void setFlipY(bool flipY);
            virtual bool getFlipY() const { return _flipY; }

            virtual void setPickable(bool pickable) { _pickable = pickable; }
            virtual bool isPickable() const { return _pickable; }

            virtual void setVisible(bool visible);
            virtual bool isVisible() const { return _visible; }

            virtual bool pointOn(const Vector2& position) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            virtual const Matrix4& getTransform() const;
            const Matrix4& getInverseTransform() const;

            virtual void updateTransform(const Matrix4& parentTransform);

            Vector2 convertWorldToLocal(const Vector2& position) const;
            Vector2 convertLocalToWorld(const Vector2& position) const;

            virtual bool checkVisibility() const;

            virtual void animate(const AnimatorPtr& animator);
            virtual AnimatorPtr getAnimator() const { return _currentAnimator; }
            virtual void stopAnimation();
            virtual void removeAnimation();

            void receiveInput(bool receiveInput) { _receiveInput = receiveInput; }
            bool isReceivingInput() const { return _receiveInput; }

            const std::vector<DrawablePtr> getDrawables() const { return _drawables; }
            void addDrawable(DrawablePtr drawable);
            void removeDrawable(uint32_t index);
            void removeDrawable(DrawablePtr drawable);

            LayerPtr getLayer() const { return _layer.lock(); }

        protected:
            virtual void addToLayer(const LayerWeakPtr& layer);
            virtual void removeFromLayer();

            virtual void calculateLocalTransform() const;
            virtual void calculateTransform() const;

            virtual void calculateInverseTransform() const;

            Matrix4 _parentTransform = Matrix4::IDENTITY;
            mutable Matrix4 _transform;
            mutable bool _transformDirty = true;
            mutable Matrix4 _inverseTransform;
            mutable bool _inverseTransformDirty = true;

            mutable Matrix4 _localTransform;
            mutable bool _localTransformDirty = true;

            mutable bool _updateChildrenTransform = true;

            //TODO: transform to parent and transform to parent

            Vector2 _position;
            float _rotation = 0.0f;
            Vector2 _scale = Vector2(1.0f, 1.0f);
            graphics::Color _color = graphics::Color(255, 255, 255, 255);
            float _opacity = 1.0f;
            float _z = 0.0f;
            bool _globalOrder = true;

            bool _flipX = false;
            bool _flipY = false;

            bool _pickable = true;
            bool _visible = true;
            bool _receiveInput = false;

            NodeContainerWeakPtr _parent;
            LayerWeakPtr _layer;

            AnimatorPtr _currentAnimator;
            bool _remove = false;

            std::vector<DrawablePtr> _drawables;
        };
    } // namespace scene
} // namespace ouzel
