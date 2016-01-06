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
        _camera.reset(new Camera());
    }
    
    Layer::~Layer()
    {
        
    }
    
    void Layer::update(float delta)
    {
        for (std::shared_ptr<Node> node : _nodes)
        {
            node->update(delta);
        }
    }
    
    void Layer::draw()
    {
        if (_reorderNodes)
        {
            std::sort(_nodes.begin(), _nodes.end(), [](std::shared_ptr<Node> const& a, std::shared_ptr<Node> const& b) {
                return a->getZ() > b->getZ();
            });
            
            _reorderNodes = false;
        }
        
        // render only if there is an active camera
        if (_camera)
        {
            for (std::shared_ptr<Node> node : _nodes)
            {
                if (node->checkVisibility())
                {
                    node->draw();
                }
            }
        }
    }
    
    void Layer::addNode(std::shared_ptr<Node> const& node)
    {
        std::vector<std::shared_ptr<Node>>::iterator i = std::find_if(_nodes.begin(), _nodes.end(), [node](std::shared_ptr<Node> const& p) {
            return p.get() == node.get();
        });
        
        if (i == _nodes.end())
        {
            _nodes.push_back(node);
            _reorderNodes = true;
        }
    }
    
    void Layer::removeNode(std::shared_ptr<Node> const& node)
    {
        std::vector<std::shared_ptr<Node>>::iterator i = std::find_if(_nodes.begin(), _nodes.end(), [node](std::shared_ptr<Node> const& p) {
            return p.get() == node.get();
        });
        
        if (i != _nodes.end())
        {
            _nodes.erase(i);
        }
    }
    
    void Layer::reorderNodes()
    {
        _reorderNodes = true;
    }
    
    void Layer::setCamera(std::shared_ptr<Camera> const& camera)
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
    
    std::shared_ptr<Node> Layer::pickNode(const Vector2& position)
    {
        for (std::vector<std::shared_ptr<Node>>::reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            std::shared_ptr<Node> node = *i;
            
            if (node->pointOn(position))
            {
                return node;
            }
        }
        
        return nullptr;
    }
    
    std::set<std::shared_ptr<Node>> Layer::pickNodes(const Rectangle& rectangle)
    {
        std::set<std::shared_ptr<Node>> result;
        
        for (std::vector<std::shared_ptr<Node>>::reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            std::shared_ptr<Node> node = *i;
            
            if (node->rectangleOverlaps(rectangle))
            {
                result.insert(node);
            }
        }
        
        return result;
    }
    
    void Layer::recalculateProjection()
    {
        Size2 size = Engine::getInstance()->getRenderer()->getSize();
        Matrix4::createOrthographic(size.width, size.height, -1.0f, 1.0f, &_projection);
        _inverseProjection = _projection;
        _inverseProjection.invert();
    }
    
    void Layer::setOrder(int32_t order)
    {
        _order = order;
        
        if (std::shared_ptr<Scene> scene = _scene.lock())
        {
            scene->reorderLayers();
        }
    }
    
    void Layer::addToScene(std::shared_ptr<Scene> const& scene)
    {
        _scene = scene;
    }
    
    void Layer::removeFromScene()
    {
        _scene.reset();
    }
}
