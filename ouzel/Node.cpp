// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Node.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Layer.h"
#include "Animator.h"
#include "Camera.h"
#include "Utils.h"
#include "MathUtils.h"

namespace ouzel
{
    Node::Node()
    {
        
    }

    Node::~Node()
    {
        
    }
    
    void Node::visit()
    {
        if (_visible)
        {
            lock();
            
            std::stable_sort(_children.begin(), _children.end(), [](const NodePtr& a, const NodePtr& b) {
                return a->getZ() > b->getZ();
            });
            
            auto i = _children.begin();
            NodePtr node;
            
            for (; i != _children.end(); ++i)
            {
                node = *i;
                
                if (node->getZ() < 0.0f)
                {
                    node->visit();
                }
                else
                {
                    break;
                }
            }
            
            LayerPtr layer = _layer.lock();
            
            // check if _parent is _layer
            bool isRoot = !_parent.owner_before(_layer) && !_layer.owner_before(_parent);
            
            if (layer && (_globalOrder || isRoot) && checkVisibility())
            {
                layer->addToDrawQueue(std::static_pointer_cast<Node>(shared_from_this()));
            }
            
            for (; i != _children.end(); ++i)
            {
                node = *i;
                node->visit();
            }
            
            unlock();
        }
    }
    
    void Node::process()
    {
        lock();
        
        auto i = _children.begin();
        NodePtr node;
        
        for (; i != _children.end(); ++i)
        {
            node = *i;
            
            if (node->getZ() < 0.0f)
            {
                if (!node->isGlobalOrder() && node->isVisible() && checkVisibility())
                {
                    node->draw();
                }
            }
            else
            {
                break;
            }
        }
        
        draw();
        
        for (; i != _children.end(); ++i)
        {
            node = *i;
            
            if (!node->isGlobalOrder() && node->isVisible() && checkVisibility())
            {
                node->draw();
            }
        }
        
        unlock();
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
        if (_currentAnimator)
        {
            _currentAnimator->update(delta);
        }
        
        lock();
        
        for (const NodePtr& child : _children)
        {
            child->update(delta);
        }
        
        unlock();
    }
    
    bool Node::addChild(const NodePtr& node)
    {
        if (NodeContainer::addChild(node))
        {
            node->addToLayer(_layer);
            
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
    
    bool Node::removeFromParent()
    {
        if (NodeContainerPtr parent = _parent.lock())
        {
            parent->removeChild(std::static_pointer_cast<Node>(shared_from_this()));
            return true;
        }
        
        return false;
    }

    void Node::setZ(float z)
    {
        _z = z;
        
        // Currently z does not affect transformation
        //markTransformDirty();
    }
    
    void Node::setGlobalOrder(bool globalOrder)
    {
        _globalOrder = globalOrder;
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
        _visible = visible;
    }

    void Node::addToLayer(const LayerWeakPtr& layer)
    {
        _layer = layer;
        
        if (LayerPtr layer = _layer.lock())
        {
            for (const NodePtr& child : _children)
            {
                child->addToLayer(layer);
            }
        }
    }

    void Node::removeFromLayer()
    {
        for (const NodePtr& child : _children)
        {
            child->removeFromLayer();
        }
        
        _layer.reset();
    }

    bool Node::pointOn(const Vector2& position) const
    {
        return _boundingBox.containPoint(convertWorldToLocal(position));
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
        
        if (localLeftBottom.x > _boundingBox.max.x ||
            localLeftBottom.y > _boundingBox.max.y ||
            localRightTop.x < _boundingBox.min.x ||
            localRightTop.y < _boundingBox.min.y)
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
        markTransformDirty();
    }
    
    Vector2 Node::convertWorldToLocal(const Vector2& position) const
    {
        Vector3 localPosition = position;
        
        const Matrix4& inverseTransform = getInverseTransform();
        inverseTransform.transformPoint(&localPosition);
        
        return Vector2(localPosition.x, localPosition.y);
    }
    
    Vector2 Node::convertLocalToWorld(const Vector2& position) const
    {
        Vector3 worldPosition = position;
        
        const Matrix4& transform = getTransform();
        transform.transformPoint(&worldPosition);
        
        return Vector2(worldPosition.x, worldPosition.y);
    }
    
    bool Node::checkVisibility() const
    {
        if (const LayerPtr& layer = _layer.lock())
        {
            if (_boundingBox.isEmpty() || _boundingRadius == 0.0f)
            {
                return true;
            }
            
            Matrix4 mvp = layer->getProjection() * layer->getCamera()->getTransform() * _transform;
            
            Vector3 position;
            
            mvp.transformPoint(&position);
            
            float radius = _boundingRadius * std::max(_scale.x, _scale.y);
            radius /= layer->getCamera()->getZoom();
            
            Vector2 radiusAxis(radius / (2.0f * Engine::getInstance()->getRenderer()->getSize().width - 1.0f),
                               radius / (2.0f * Engine::getInstance()->getRenderer()->getSize().height - 1.0f));
            
            if (position.x >= -(1.0f + radiusAxis.x) && position.x <= (1.0f + radiusAxis.x) && position.y >= -(1.0f + radiusAxis.y) && position.y <= (1.0f + radiusAxis.y))
            {
                /*Vector3 corners[4] = {
                    Vector3(_boundingBox.min.x, _boundingBox.min.y, 0.0f),
                    Vector3(_boundingBox.max.x, _boundingBox.min.y, 0.0f),
                    Vector3(_boundingBox.max.x, _boundingBox.max.y, 0.0f),
                    Vector3(_boundingBox.min.x, _boundingBox.max.y, 0.0f)
                };
                
                uint8_t inCorners = 0;
                
                for (Vector3& corner : corners)
                {
                    mvp.transformPoint(&corner);
                    
                    if (corner.x >= -1.0f && corner.x <= 1.0f && corner.y >= -1.0f && corner.y <= 1.0f)
                    {
                        return true;
                    }
                    
                    if (corner.x < -1.0f && corner.y < -1.0f) inCorners |= 0x01;
                    if (corner.x > 1.0f && corner.y < -1.0f) inCorners |= 0x02;
                    if (corner.x > 1.0f && corner.y > 1.0f) inCorners |= 0x04;
                    if (corner.x < -1.0f && corner.y > 1.0f) inCorners |= 0x08;
                }
                
                // bounding box is bigger than screen
                if (inCorners == 0x0F)
                {
                    return true;
                }
                
                for (uint32_t current = 0; current < 4; ++current)
                {
                    uint32_t next = (current == 3) ? 0 : current + 1;
                    
                    if (linesIntersect(Vector2(corners[current].x, corners[current].y), Vector2(corners[next].x, corners[next].y), Vector2(-1.0f, 1.0f), Vector2(1.0f, 1.0f)) || // top
                        linesIntersect(Vector2(corners[current].x, corners[current].y), Vector2(corners[next].x, corners[next].y), Vector2(-1.0f, -1.0f), Vector2(1.0f, -1.0f)) || // bottom
                        linesIntersect(Vector2(corners[current].x, corners[current].y), Vector2(corners[next].x, corners[next].y), Vector2(1.0f, -1.0f), Vector2(1.0f, 1.0f)) || // right
                        linesIntersect(Vector2(corners[current].x, corners[current].y), Vector2(corners[next].x, corners[next].y), Vector2(-1.0f, -1.0f), Vector2(-1.0f, 1.0f))) // left
                    {
                        return true;
                    }
                }*/
                
                return true;
            }
        }
        
        return false;
    }
    
    void Node::animate(const AnimatorPtr& animator)
    {
        _currentAnimator = animator;
        
        if (_currentAnimator)
        {
            _currentAnimator->start(std::static_pointer_cast<Node>(shared_from_this()));
        }
    }
    
    void Node::stopAnimation()
    {
        if (_currentAnimator)
        {
            _currentAnimator->stop();
            _currentAnimator.reset();
        }
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
        
        for (const NodePtr& child : _children)
        {
            child->updateTransform(_transform);
        }
    }
    
    void Node::calculateTransformRecursive() const
    {
        if (_transformDirty)
        {
            calculateTransform();
        }
        
        for (const NodePtr& child : _children)
        {
            child->calculateTransformRecursive();
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
