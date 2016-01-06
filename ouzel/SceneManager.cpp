// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SceneManager.h"
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
        sharedSceneManager = nullptr;
    }
    
    void SceneManager::update(float delta)
    {
        if (_scene)
        {
            _scene->update(delta);
        }
    }
    
    void SceneManager::setScene(std::shared_ptr<Scene> const& scene)
    {
        if (scene != _scene)
        {
            _scene = scene;
            _scene->recalculateProjection();
        }
    }
    
    void SceneManager::draw()
    {
        if (_scene)
        {
            _scene->draw();
        }
    }
    
    void SceneManager::recalculateProjection()
    {
        if (_scene)
        {
            _scene->recalculateProjection();
        }
        
    }
}
