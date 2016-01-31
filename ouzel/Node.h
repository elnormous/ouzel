// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "Types.h"
#include "NodeContainer.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "Rectangle.h"
#include "AABB2.h"

namespace ouzel
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
        
        virtual void draw();
        virtual void update(float delta);
        
        virtual bool addChild(const NodePtr& node) override;
        virtual bool hasParent() const { return !_parent.expired(); }
        virtual NodeContainerPtr getParent() const { return _parent.lock(); }
        virtual bool removeFromParent();
        
        virtual void setZ(float z);
        virtual float getZ() const { return _z; }
        
        virtual void setPosition(const Vector2& position);
        virtual const Vector2& getPosition() const { return _position; }
        
        virtual void setRotation(float rotation);
        virtual float getRotation() const { return _rotation; }
        
        virtual void setScale(const Vector2& scale);
        virtual const Vector2& getScale() const { return _scale; }
        
        virtual void setOpacity(float opacity) { _opacity = opacity; }
        virtual float getOpacity() const { return _opacity; }
        
        virtual void setFlipX(bool flipX);
        virtual bool getFlipX() const { return _flipX; }
        
        virtual void setFlipY(bool flipY);
        virtual bool getFlipY() const { return _flipY; }
        
        virtual void setVisible(bool visible);
        virtual bool isVisible() const { return _visible; }
        
        virtual const AABB2& getBoundingBox() const { return _boundingBox; }
        
        virtual bool pointOn(const Vector2& position) const;
        virtual bool rectangleOverlaps(const Rectangle& rectangle) const;
        
        virtual const Matrix4& getTransform() const;
        const Matrix4& getInverseTransform() const;
        
        virtual void updateTransform(const Matrix4& parentTransform);
        
        Vector2 convertWorldToLocal(const Vector2& position) const;
        Vector2 convertLocalToWorld(const Vector2& position) const;
        
        virtual bool checkVisibility() const;
        
        virtual void animate(const AnimatorPtr& animator);
        virtual void stopAnimation();
        
    protected:
        virtual void addToLayer(const LayerWeakPtr& layer);
        virtual void removeFromLayer();
        
        virtual void calculateTransform() const;
        virtual void calculateTransformRecursive() const;
        
        void markTransformDirty() const;
        virtual void calculateInverseTransform() const;
        
        bool isParentVisible() const { return _parentVisible; }
        void setParentVisible(bool parentVisible);
        
        Matrix4 _parentTransform = Matrix4::identity();
        mutable Matrix4 _transform;
        mutable bool _transformDirty = false;
        mutable Matrix4 _inverseTransform;
        mutable bool _inverseTransformDirty = false;
        
        Vector2 _position;
        float _rotation = 0.0f;
        Vector2 _scale = Vector2(1.0f, 1.0f);
        float _z = 0.0f;
        float _opacity = 1.0f;
        
        AABB2 _boundingBox;
        float _boundingRadius = 0.0f;
        
        bool _flipX = false;
        bool _flipY = false;
        
        bool _visible = true;
        bool _parentVisible = false;
        
        NodeContainerWeakPtr _parent;
        LayerWeakPtr _layer;
        
        AnimatorPtr _currentAnimator;
    };
}
