// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "AutoPtr.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "Rectangle.h"

namespace ouzel
{
    class Scene;

    class Node: public Noncopyable, public ReferenceCounted
    {
        friend Scene;
    public:
        Node();
        virtual ~Node();
        
        virtual void addChild(Node* node);
        virtual void removeChild(Node* node);
        
        virtual Node* getParent() const { return _parent; }
        virtual bool hasChild(Node* node) const;
        virtual const std::vector<AutoPtr<Node>>& getChildren() const { return _children; }
        
        virtual void draw();
        virtual void update(float delta);
        
        virtual void setZOrder(float zOrder);
        virtual float getZOrder() const { return _zOrder; }
        
        virtual void setPosition(const Vector2& position);
        virtual const Vector2& getPosition() const { return _position; }
        
        virtual void setRotation(float rotation);
        virtual float getRotation() const { return _rotation; }
        
        virtual void setScale(const Vector2& scale);
        virtual const Vector2& getScale() const { return _scale; }
        
        virtual void setFlipX(bool flipX);
        virtual bool getFlipX() const { return _flipX; }
        
        virtual void setFlipY(bool flipY);
        virtual bool getFlipY() const { return _flipY; }
        
        virtual const Matrix4& getTransform() const { return _transform; }
        const Matrix4& getInverseTransform() const;
        
        virtual const Rectangle& getBoundingBox() const { return _boundingBox; }
        
        virtual bool isAddedToScene() const { return _addedToScene; }
        
        virtual bool pointOn(const Vector2& position) const;
        virtual bool rectangleOverlaps(const Rectangle& rectangle) const;
        
        virtual void updateTransform();
        
        virtual bool checkVisibility() const;
        
    protected:
        virtual void addToScene();
        virtual void removeFromScene();
        void markInverseTransformDirty();
        
        Matrix4 _transform;
        
        Vector2 _position;
        float _rotation = 0.0f;
        Vector2 _scale = Vector2(1.0f, 1.0f);
        float _zOrder = 0.0f;
        
        Rectangle _boundingBox;
        
        Node* _parent = nullptr;
        std::vector<AutoPtr<Node>> _children;
        
        bool _flipX = false;
        bool _flipY = false;
        
        bool _addedToScene = false;
        
    private:
        mutable Matrix4 _inverseTransform;
        mutable bool _inverseTransformDirty = false;
    };
}
