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
        
    }

    void Node::draw()
    {
        if (_transformDirty)
        {
            calculateTransform();
        }
    }
    
    void Node::update(float delta)
    {
        if (_transformDirty)
        {
            calculateTransform();
        }
    }
    
    bool Node::addChild(std::shared_ptr<Node> const& node)
    {
        if (NodeContainer::addChild(node))
        {
            node->_hasParent = true;
            node->setLayer(_layer);
            
            if (node->getVisible())
            {
                node->addToLayer();
            }
            
            if (_transformDirty)
            {
                calculateTransform();
            }
            else
            {
                node->updateTransform(_transform);
            }
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool Node::removeChild(std::shared_ptr<Node> const& node)
    {
        if (NodeContainer::removeChild(node))
        {
            node->_hasParent = false;
            node->_layer.reset();
            
            if (node->_addedToLayer)
            {
                node->removeFromLayer();
            }
            
            return true;
        }
        else
        {
            return false;
        }
    }

    void Node::setZ(float z)
    {
        _z = z;
        
        if (std::shared_ptr<Layer> layer = _layer.lock())
        {
            layer->reorderNodes();
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
    
    void Node::setVisible(bool visible)
    {
        if (visible != _visible)
        {
            _visible = visible;
            
            if (_visible)
            {
                addToLayer();
            }
            else
            {
                removeFromLayer();
            }
        }
    }

    void Node::addToLayer()
    {
        if (_visible)
        {
            _addedToLayer = true;
            
            if (std::shared_ptr<Layer> layer = _layer.lock())
            {
                layer->addNode(shared_from_this());
                
                for (std::shared_ptr<Node> child : _children)
                {
                    child->addToLayer();
                }
            }
        }
    }

    void Node::removeFromLayer()
    {
        _addedToLayer = false;
        
        if (std::shared_ptr<Layer> layer = _layer.lock())
        {
            layer->removeNode(shared_from_this());
            
            for (std::shared_ptr<Node> child : _children)
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
    
    const Matrix4& Node::getTransform() const
    {
        if (_transformDirty)
        {
            calculateTransform();
        }
        
        return _transform;
    }
    
    const Matrix4& Node::getInverseTransform() const
    {
        if (_transformDirty)
        {
            calculateTransform();
        }
        
        if (_inverseTransformDirty)
        {
            calculateInverseTransform();
        }
        
        return _inverseTransform;
    }

    void Node::updateTransform(const Matrix4& parentTransform)
    {
        _parentTransform = parentTransform;
        calculateTransform();
        _inverseTransformDirty = true;
    }
    
    void Node::setLayer(std::weak_ptr<Layer> const& layer)
    {
        _layer = layer;
        
        for (auto& node : _children)
        {
            node->setLayer(layer);
        }
    }
    
    bool Node::checkVisibility() const
    {
        return true;
    }
    
    void Node::calculateTransform() const
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
        
        _transform = _parentTransform * translation * rotation * scale;
        _transformDirty = false;
        
        for (std::shared_ptr<Node> child : _children)
        {
            child->updateTransform(_transform);
        }
    }
    
    void Node::calculateInverseTransform() const
    {
        if (_transformDirty)
        {
            calculateTransform();
        }
        
        _inverseTransform = _transform;
        _inverseTransform.invert();
        _inverseTransformDirty = false;
    }
    
    void Node::markTransformDirty() const
    {
        _transformDirty = true;
        _inverseTransformDirty = true;
    }
}
