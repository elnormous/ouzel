// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "SceneManager.h"
#include "Engine.h"
#include "Camera.h"
#include "Scene.h"

namespace ouzel
{
    static SceneManager* sharedSceneManager = nullptr;

    SceneManager* SceneManager::getInstance()
    {
        return sharedSceneManager;
    }

    SceneManager::SceneManager()
    {
        sharedSceneManager = this;
    }
    
    SceneManager::~SceneManager()
    {
        
    }
    
    bool SceneManager::init()
    {
        _camera = new Camera();
        
        _rootNode = new Node();
        _rootNode->addToScene();
        
        return true;
    }
    
    void SceneManager::update(float delta)
    {
        if (_camera) _camera->updateTransform(Matrix4::identity());
        _rootNode->updateTransform(Matrix4::identity());
        
        for (std::vector<AutoPtr<Node>>::reverse_iterator i = _nodes.rbegin(); i != _nodes.rend(); ++i)
        {
            Node* node = *i;
            
            node->update(delta);
        }
    }
    
    void SceneManager::setScene(Scene* scene)
    {
        _scene = scene;
    }
    
    void SceneManager::addNode(Node* node)
    {
        std::vector<AutoPtr<Node>>::iterator i = std::find(_nodes.begin(), _nodes.end(), node);
        
        if (i == _nodes.end())
        {
            _nodes.push_back(node);
            _reorderNodes = true;
        }
    }
    
    void SceneManager::removeNode(Node* node)
    {
        std::vector<AutoPtr<Node>>::iterator i = std::find(_nodes.begin(), _nodes.end(), node);
        
        if (i != _nodes.end())
        {
            _nodes.erase(i);
        }
    }
    
    void SceneManager::reorderNodes()
    {
        _reorderNodes = true;
    }
    
    void SceneManager::setCamera(Camera* camera)
    {
        _camera = camera;
    }
    
    Node* SceneManager::pickNode(const Vector2& position)
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
    
    std::set<Node*> SceneManager::pickNodes(const Rectangle& rectangle)
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
    
    void SceneManager::drawAll()
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
}
