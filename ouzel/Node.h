// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "NodeContainer.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "Rectangle.h"

namespace ouzel
{
    class SceneManager;

    class Node: public NodeContainer, public std::enable_shared_from_this<Node>
    {
        friend SceneManager;
        friend NodeContainer;
    public:
        Node();
        virtual ~Node();
        
        virtual std::shared_ptr<NodeContainer> getParent() const { return _parent.lock(); }
        
        virtual void draw();
        virtual void update(float delta);
        
        virtual void addChild(std::shared_ptr<Node> const& node) override;
        
        virtual void setZ(float z);
        virtual float getZ() const { return _z; }
        
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
        
        virtual void setVisible(bool visible);
        virtual bool getVisible() const { return _visible; }
        
        virtual const Rectangle& getBoundingBox() const { return _boundingBox; }
        
        virtual bool pointOn(const Vector2& position) const;
        virtual bool rectangleOverlaps(const Rectangle& rectangle) const;
        
        virtual const Matrix4& getTransform() const;
        const Matrix4& getInverseTransform() const;
        
        virtual void updateTransform(const Matrix4& parentTransform);
        
        virtual bool checkVisibility() const;
        
    protected:
        virtual void addToLayer();
        virtual void removeFromLayer();
        
        virtual void calculateTransform() const;
        void markTransformDirty() const;
        virtual void calculateInverseTransform() const;
        
        Matrix4 _parentTransform = Matrix4::identity();
        mutable Matrix4 _transform;
        mutable bool _transformDirty = false;
        mutable Matrix4 _inverseTransform;
        mutable bool _inverseTransformDirty = false;
        
        Vector2 _position;
        float _rotation = 0.0f;
        Vector2 _scale = Vector2(1.0f, 1.0f);
        float _z = 0.0f;
        
        Rectangle _boundingBox;
        
        bool _flipX = false;
        bool _flipY = false;
        
        bool _visible = true;
        
        std::weak_ptr<Node> _parent;
    };
}
