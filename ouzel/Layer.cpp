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
        
        for (const NodePtr& node : _nodes)
        {
            node->update(delta);
        }
        
        unlock();
    }
    
    void Layer::draw()
    {
        if (_reorderNodes)
        {
            std::sort(_nodes.begin(), _nodes.end(), [](const NodePtr& a, const NodePtr& b) {
                return a->getZ() > b->getZ();
            });
            
            _reorderNodes = false;
        }
        
        // render only if there is an active camera
        if (_camera)
        {
            lock();
            
            for (const NodePtr& child : _children)
            {
                child->calculateTransformRecursive();
            }
            
            for (const NodePtr& node : _nodes)
            {
                if (node->isVisible() && node->isParentVisible() && node->checkVisibility())
                {
                    node->draw();
                }
            }
            
            unlock();
        }
    }
    
    bool Layer::addChild(const NodePtr& node)
    {
        if (NodeContainer::addChild(node))
        {
            node->addToLayer(std::static_pointer_cast<Layer>(shared_from_this()));
            node->setParentVisible(true);
            
            node->updateTransform(Matrix4::identity());
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void Layer::addNode(const NodePtr& node)
    {
        std::vector<NodePtr>::iterator i = std::find_if(_nodes.begin(), _nodes.end(), [node](const NodePtr& p) {
            return p.get() == node.get();
        });
        
        if (i == _nodes.end())
        {
            _nodes.push_back(node);
            _reorderNodes = true;
        }
    }
    
    void Layer::removeNode(const NodePtr& node)
    {
        if (_locked)
        {
            _nodeDeleteList.insert(node);
        }
        else
        {
            std::vector<NodePtr>::iterator i = std::find_if(_nodes.begin(), _nodes.end(), [node](const NodePtr& p) {
                return p.get() == node.get();
            });
            
            if (i != _nodes.end())
            {
                _nodes.erase(i);
            }
        }
    }
    
    void Layer::reorderNodes()
    {
        _reorderNodes = true;
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
        for (std::vector<NodePtr>::const_reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            NodePtr node = *i;
            
            if (node->pointOn(position))
            {
                return node;
            }
        }
        
        return nullptr;
    }
    
    std::set<NodePtr> Layer::pickNodes(const Rectangle& rectangle)
    {
        std::set<NodePtr> result;
        
        for (std::vector<NodePtr>::const_reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            NodePtr node = *i;
            
            if (node->rectangleOverlaps(rectangle))
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
    
    void Layer::lock()
    {
        _locked = true;
    }
    
    void Layer::unlock()
    {
        _locked = false;
        
        for (const NodePtr& node : _nodeDeleteList)
        {
            removeNode(node);
        }
        
        _nodeDeleteList.clear();
    }
}
