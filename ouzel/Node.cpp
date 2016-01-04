// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Node.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Layer.h"

namespace ouzel
{
    Node::Node()
    {
        
    }

    Node::~Node()
    {
        for (AutoPtr<Node> node : _children)
        {
            node->_parent = nullptr;
        }
    }

    void Node::draw()
    {

    }
    
    void Node::update(float delta)
    {
        
    }

    void Node::setZOrder(float zOrder)
    {
        _zOrder = zOrder;
        
        if (_layer)
        {
            _layer->reorderNodes();
        }
        
        markTransformDirty();
    }

    void Node::setPosition(const Vector2& position)
    {
        _position = position;
        
        markTransformDirty();
    }

    void Node::setRotation(float rotation)
    {
        _rotation = rotation;
        
        markTransformDirty();
    }

    void Node::setScale(const Vector2& scale)
    {
        _scale = scale;
        
        markTransformDirty();
    }
    
    void Node::setFlipX(bool flipX)
    {
        _flipX = flipX;
        
        markTransformDirty();
    }
    
    void Node::setFlipY(bool flipY)
    {
        _flipY = flipY;
        
        markTransformDirty();
    }

    void Node::addToLayer(Layer* layer)
    {
        if (layer && _layer != layer)
        {
            removeFromLayer();
            
            _layer = layer;
            _layer->addNode(this);
            
            for (AutoPtr<Node> child : _children)
            {
                child->addToLayer(layer);
            }
        }
    }

    void Node::removeFromLayer()
    {
        if (_layer)
        {
            _layer->removeNode(this);
            _layer = nullptr;
            
            for (AutoPtr<Node> child : _children)
            {
                child->removeFromLayer();
            }
        }
    }

    bool Node::pointOn(const Vector2& position) const
    {
        Vector3 localPosition = Vector3(position.x, position.y, 0.0f);
        
        const Matrix4& inverseTransform = getInverseTransform();
        inverseTransform.transformPoint(&localPosition);
        
        return _boundingBox.contains(localPosition.x, localPosition.y);
    }
    
    bool Node::rectangleOverlaps(const Rectangle& rectangle) const
    {
        if (_boundingBox.isEmpty())
        {
            return false;
        }
        
        Vector3 localLeftBottom = Vector3(rectangle.x, rectangle.y, 0.0f);
        
        const Matrix4& inverseTransform = getInverseTransform();
        inverseTransform.transformPoint(&localLeftBottom);
        
        Vector3 localRightTop =  Vector3(rectangle.x + rectangle.width, rectangle.y + rectangle.height, 0.0f);
        
        inverseTransform.transformPoint(&localRightTop);
        
        if (localLeftBottom.x > _boundingBox.x + _boundingBox.width ||
            localLeftBottom.y > _boundingBox.y + _boundingBox.height ||
            localRightTop.x < _boundingBox.x ||
            localRightTop.y < _boundingBox.y)
        {
            return false;
        }
        
        return true;
    }

    void Node::updateTransform(const Matrix4& parentTransform)
    {
        if (_transformDirty)
        {
            Matrix4 translation;
            translation.translate(Vector3(_position.x, _position.y, 0.0f));
            
            Matrix4 rotation;
            rotation.rotate(Vector3(0.0f, 0.0f, -1.0f), _rotation);
            
            Vector3 realScale = Vector3(_scale.x * (_flipX ? -1.0f : 1.0f),
                                        _scale.y * (_flipY ? -1.0f : 1.0f),
                                        1.0f);
            
            Matrix4 scale;
            scale.scale(realScale);
            
            _transform = parentTransform * translation * rotation * scale;
            _inverseTransform = _transform;
            _inverseTransform.invert();
            
            _transformDirty = false;
        }
        
        for (AutoPtr<Node> child : _children)
        {
            child->updateTransform(_transform);
        }
    }
    
    bool Node::checkVisibility() const
    {
        return true;
    }
    
    void Node::markTransformDirty()
    {
        _transformDirty = true;
    }
}
