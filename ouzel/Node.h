// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "NodeContainer.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "Rectangle.h"

namespace ouzel
{
    class SceneManager;

    class Node: public NodeContainer
    {
        friend SceneManager;
        friend NodeContainer;
    public:
        Node();
        virtual ~Node();
        
        virtual NodeContainer* getParent() const { return _parent; }
        
        virtual void draw();
        virtual void update(float delta);
        
        virtual void addChild(Node* node) override;
        
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
        
        virtual const Rectangle& getBoundingBox() const { return _boundingBox; }
        
        virtual bool pointOn(const Vector2& position) const;
        virtual bool rectangleOverlaps(const Rectangle& rectangle) const;
        
        virtual const Matrix4& getTransform() const;
        const Matrix4& getInverseTransform() const;
        
        virtual void updateTransform(const Matrix4& parentTransform);
        
        virtual bool checkVisibility() const;
        
    protected:
        virtual void addToLayer(Layer* layer);
        virtual void removeFromLayer();
        
        virtual void calculateTransform() const;
        void markTransformDirty() const;
        virtual void calculateInverseTransform() const;
        
        NodeContainer* _parent = nullptr;
        
        Matrix4 _parentTransform = Matrix4::identity();
        mutable Matrix4 _transform;
        mutable bool _transformDirty = false;
        mutable Matrix4 _inverseTransform;
        mutable bool _inverseTransformDirty = false;
        
        Vector2 _position;
        float _rotation = 0.0f;
        Vector2 _scale = Vector2(1.0f, 1.0f);
        float _zOrder = 0.0f;
        
        Rectangle _boundingBox;
        
        bool _flipX = false;
        bool _flipY = false;
    };
}
