// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Node.h"
#include "Engine.h"
#include "Scene.h"

namespace ouzel
{
    Node::Node(Scene* scene):
        _scene(scene)
    {
        
    }

    Node::~Node()
    {
        for (AutoPtr<Node> node : _children)
        {
            if (_addedToRenderer)
            {
                node->removeFromRenderer();
            }
            
            node->_parent = nullptr;
        }
        
        _children.clear();
    }

    bool Node::hasChild(Node* node) const
    {
        std::vector<AutoPtr<Node>>::const_iterator i = std::find(_children.begin(), _children.end(), node);
        
        return i != _children.end();
    }

    void Node::addChild(Node* node)
    {
        if (!hasChild(node) && node->getParent() == nullptr)
        {
            if (_addedToRenderer)
            {
                node->addToRenderer();
            }
            
            _children.push_back(node);
            node->_parent = this;
            node->updateTransform();
            node->retain();
        }
    }

    void Node::removeChild(Node* node)
    {
        std::vector<AutoPtr<Node>>::iterator i = std::find(_children.begin(), _children.end(), node);
        
        if (i != _children.end())
        {
            if (_addedToRenderer)
            {
                node->removeFromRenderer();
            }
            
            node->_parent = nullptr;
            _children.erase(i);
        }
    }

    void Node::draw()
    {

    }

    void Node::setZOrder(float zOrder)
    {
        _zOrder = zOrder;
        _scene->reorderNodes();
        
        updateTransform();
    }

    void Node::setPosition(const Vector2& position)
    {
        _position = position;
        
        updateTransform();
    }

    void Node::setRotation(float rotation)
    {
        _rotation = rotation;
        
        updateTransform();
    }

    void Node::setScale(const Vector2& scale)
    {
        _scale = scale;
        
        updateTransform();
    }
    
    void Node::setFlipX(bool flipX)
    {
        _flipX = flipX;
        
        updateTransform();
    }
    
    void Node::setFlipY(bool flipY)
    {
        _flipY = flipY;
        
        updateTransform();
    }

    void Node::addToRenderer()
    {
        _scene->addNode(this);
        _addedToRenderer = true;
        
        for (AutoPtr<Node> child : _children)
        {
            child->addToRenderer();
        }
    }

    void Node::removeFromRenderer()
    {
        _scene->removeNode(this);
        _addedToRenderer = false;
        
        for (AutoPtr<Node> child : _children)
        {
            child->removeFromRenderer();
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

    void Node::updateTransform()
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
        
        _transform = translation * rotation * scale;
        
        if (_parent)
        {
            _transform = _parent->_transform * _transform;
        }
        
        markInverseTransformDirty();
        
        for (AutoPtr<Node> child : _children)
        {
            child->updateTransform();
        }
    }
    
    bool Node::checkVisibility() const
    {
        return true;
    }
    
    void Node::markInverseTransformDirty()
    {
        _inverseTransformDirty = true;
    }
    
    const Matrix4& Node::getInverseTransform() const
    {
        if (_inverseTransformDirty)
        {
            _inverseTransform = _transform;
            _inverseTransform.invert();
            _inverseTransformDirty = false;
        }
        
        return _inverseTransform;
    }
}
