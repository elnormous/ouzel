// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Scene.h"
#include "Engine.h"
#include "Camera.h"

namespace ouzel
{
    Scene::Scene(Engine* engine)
    {
        _engine = engine;
    }
    
    Scene::~Scene()
    {
        if (!_camera) _camera->release();
        if (!_rootNode) _rootNode->release();
    }
    
    bool Scene::init()
    {
        _camera = new Camera(this);
        
        _rootNode = new Node(this);
        _rootNode->addToRenderer();
        
        return true;
    }
    
    void Scene::addNode(Node* node)
    {
        std::vector<Node*>::iterator i = std::find(_nodes.begin(), _nodes.end(), node);
        
        if (i == _nodes.end())
        {
            _nodes.push_back(node);
            _reorderNodes = true;
        }
    }
    
    void Scene::removeNode(Node* node)
    {
        std::vector<Node*>::iterator i = std::find(_nodes.begin(), _nodes.end(), node);
        
        if (i != _nodes.end())
        {
            _nodes.erase(i);
        }
    }
    
    void Scene::reorderNodes()
    {
        _reorderNodes = true;
    }
    
    void Scene::setCamera(Camera* camera)
    {
        if (_camera)
        {
            _camera->release();
        }
        
        _camera = camera;
        
        if (_camera)
        {
            _camera->retain();
        }
    }
    
    Node* Scene::pickNode(const Vector2& position)
    {
        for (std::vector<Node*>::reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            Node* node = *i;
            
            if (node->pointOn(position))
            {
                return node;
            }
        }
        
        return nullptr;
    }
    
    std::set<Node*> Scene::pickNodes(const Rectangle& rectangle)
    {
        std::set<Node*> result;
        
        for (std::vector<Node*>::reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            Node* node = *i;
            
            if (node->rectangleOverlaps(rectangle))
            {
                result.insert(node);
            }
        }
        
        return result;
    }
    
    void Scene::drawAll()
    {        
        if (_reorderNodes)
        {
            std::sort(_nodes.begin(), _nodes.end(), [](Node* a, Node* b){
                return a->getZOrder() < b->getZOrder();
            });
            
            _reorderNodes = false;
        }
        
        for (Node* node : _nodes)
        {
            if (node->checkVisibility())
            {
                node->draw();
            }
        }
    }
}
