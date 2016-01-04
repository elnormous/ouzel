// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Layer.h"
#include "Node.h"
#include "Camera.h"
#include "Renderer.h"
#include "Matrix4.h"

namespace ouzel
{
    Layer::Layer()
    {
        _camera = new Camera();
        _layer = this;
    }
    
    Layer::~Layer()
    {
        
    }
    
    void Layer::update(float delta)
    {
        for (std::vector<AutoPtr<Node>>::reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            Node* node = *i;
            
            node->update(delta);
        }
    }
    
    void Layer::draw()
    {
        if (_reorderNodes)
        {
            std::sort(_nodes.begin(), _nodes.end(), [](Node* a, Node* b){
                return a->getZOrder() < b->getZOrder();
            });
            
            _reorderNodes = false;
        }
        
        // render only if there is an active camera
        if (_camera)
        {
            for (AutoPtr<Node> node : _nodes)
            {
                if (node->checkVisibility())
                {
                    node->draw();
                }
            }
        }
    }
    
    void Layer::addNode(Node* node)
    {
        std::vector<AutoPtr<Node>>::iterator i = std::find(_nodes.begin(), _nodes.end(), node);
        
        if (i == _nodes.end())
        {
            _nodes.push_back(node);
            _reorderNodes = true;
        }
    }
    
    void Layer::removeNode(Node* node)
    {
        std::vector<AutoPtr<Node>>::iterator i = std::find(_nodes.begin(), _nodes.end(), node);
        
        if (i != _nodes.end())
        {
            _nodes.erase(i);
        }
    }
    
    void Layer::reorderNodes()
    {
        _reorderNodes = true;
    }
    
    void Layer::setCamera(Camera* camera)
    {
        _camera = camera;
    }
    
    Vector2 Layer::viewToWorldLocation(const Vector2& position)
    {
        if (_camera)
        {
            Matrix4 projViewMatrix = Renderer::getInstance()->getProjection() * _camera->getTransform();
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
    
    Vector2 Layer::worldToViewLocation(const Vector2& position)
    {
        if (_camera)
        {
            Matrix4 projViewMatrix = Renderer::getInstance()->getProjection() * _camera->getTransform();
            
            Vector3 result = Vector3(position.x, position.y, 0.0f);
            projViewMatrix.transformPoint(&result);
            
            return Vector2(result.x, result.y);
        }
        else
        {
            return Vector2();
        }
    }
    
    Node* Layer::pickNode(const Vector2& position)
    {
        for (std::vector<AutoPtr<Node>>::reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            Node* node = *i;
            
            if (node->pointOn(position))
            {
                return node;
            }
        }
        
        return nullptr;
    }
    
    std::set<Node*> Layer::pickNodes(const Rectangle& rectangle)
    {
        std::set<Node*> result;
        
        for (std::vector<AutoPtr<Node>>::reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            Node* node = *i;
            
            if (node->rectangleOverlaps(rectangle))
            {
                result.insert(node);
            }
        }
        
        return result;
    }
}
