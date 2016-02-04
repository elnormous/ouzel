// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Engine.h"
#include "Layer.h"
#include "Node.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4.h"

namespace ouzel
{
    Layer::Layer()
    {
        _camera = std::make_shared<Camera>();
    }
    
    Layer::~Layer()
    {
        
    }
    
    void Layer::update(float delta)
    {
        lock();
        
        for (const NodePtr& child : _children)
        {
            child->update(delta);
        }
        
        unlock();
    }
    
    void Layer::draw()
    {
        _drawQueue.clear();
        
        // render only if there is an active camera
        if (_camera)
        {
            lock();
            
            for (const NodePtr child : _children)
            {
                child->visit();
                child->calculateTransformRecursive();
            }
            
            std::stable_sort(_drawQueue.begin(), _drawQueue.end(), [](const NodePtr& a, const NodePtr& b) {
                return a->getZ() > b->getZ();
            });
            
            for (const NodePtr& node : _drawQueue)
            {
                node->process();
            }
            
            unlock();
        }
    }
    
    bool Layer::addChild(const NodePtr& node)
    {
        if (NodeContainer::addChild(node))
        {
            node->addToLayer(std::static_pointer_cast<Layer>(shared_from_this()));
            
            node->updateTransform(Matrix4::identity());
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void Layer::addToDrawQueue(const NodePtr& node)
    {
        _drawQueue.push_back(node);
    }
    
    void Layer::setCamera(const CameraPtr& camera)
    {
        _camera = camera;
    }
    
    Vector2 Layer::screenToWorldLocation(const Vector2& position)
    {
        if (_camera)
        {
            Matrix4 projViewMatrix = _projection * _camera->getTransform();
            Matrix4 inverseViewMatrix = projViewMatrix;
            inverseViewMatrix.invert();
            
            Vector3 result = Vector3(position.x, position.y, 0.0f);
            inverseViewMatrix.transformPoint(&result);
            
            return Vector2(result.x, result.y);
        }
        else
        {
            return Vector2();
        }
    }
    
    Vector2 Layer::worldToScreenLocation(const Vector2& position)
    {
        if (_camera)
        {
            Matrix4 projViewMatrix = _projection * _camera->getTransform();
            
            Vector3 result = Vector3(position.x, position.y, 0.0f);
            projViewMatrix.transformPoint(&result);
            
            return Vector2(result.x, result.y);
        }
        else
        {
            return Vector2();
        }
    }
    
    NodePtr Layer::pickNode(const Vector2& position)
    {
        for (std::vector<NodePtr>::const_reverse_iterator i = _drawQueue.rbegin(); i != _drawQueue.rend(); ++i)
        {
            NodePtr node = *i;
            
            if (node->isPickable() && node->pointOn(position))
            {
                return node;
            }
        }
        
        return nullptr;
    }
    
    std::set<NodePtr> Layer::pickNodes(const Rectangle& rectangle)
    {
        std::set<NodePtr> result;
        
        for (std::vector<NodePtr>::const_reverse_iterator i = _drawQueue.rbegin(); i != _drawQueue.rend(); ++i)
        {
            NodePtr node = *i;
            
            if (node->isPickable() && node->rectangleOverlaps(rectangle))
            {
                result.insert(node);
            }
        }
        
        return result;
    }
    
    void Layer::setScaleMode(ScaleMode scaleMode)
    {
        _scaleMode = scaleMode;
        recalculateProjection();
    }
    
    void Layer::setDesignSize(const Size2& designSize)
    {
        _designSize = designSize;
        recalculateProjection();
    }
    
    void Layer::recalculateProjection()
    {
        Size2 size = Engine::getInstance()->getRenderer()->getSize();
        
        if (_designSize.width != 0.0f && _designSize.height != 0.0f &&
            size.width != 0.0f && size.height != 0.0f)
        {
            float aspectRatio = size.width / size.height;
            
            switch (_scaleMode)
            {
                case ScaleMode::None:
                {
                    // Do nothing
                    break;
                }
                case ScaleMode::ExactFit:
                {
                    size.width = _designSize.width;
                    size.height = _designSize.height;
                    break;
                }
                case ScaleMode::NoBorder:
                {
                    if (size.width / size.height > _designSize.width / _designSize.height)
                    {
                        size.width = _designSize.width;
                        size.height = _designSize.width / aspectRatio;
                    }
                    else
                    {
                        size.width = _designSize.height * aspectRatio;
                        size.height = _designSize.height;
                    }
                    break;
                }
                case ScaleMode::ShowAll:
                {
                    if (size.width / size.height < _designSize.width / _designSize.height)
                    {
                        size.width = _designSize.width;
                        size.height = _designSize.width / aspectRatio;
                    }
                    else
                    {
                        size.width = _designSize.height * aspectRatio;
                        size.height = _designSize.height;
                    }
                    break;
                }
            }
        }
        
        Matrix4::createOrthographic(size.width, size.height, -1.0f, 1.0f, &_projection);
        _inverseProjection = _projection;
        _inverseProjection.invert();
    }
    
    void Layer::setOrder(int32_t order)
    {
        _order = order;
        
        if (ScenePtr scene = _scene.lock())
        {
            scene->reorderLayers();
        }
    }
    
    void Layer::addToScene(const ScenePtr& scene)
    {
        _scene = scene;
    }
    
    void Layer::removeFromScene()
    {
        _scene.reset();
    }
}
