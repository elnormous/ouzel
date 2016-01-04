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
        
    }
    
    void SceneManager::update(float delta)
    {
        _scene->update(delta);
    }
    
    void SceneManager::setScene(Scene* scene)
    {
        if (scene != _scene)
        {
            _scene = scene;
            _scene->recalculateProjection();
        }
    }
    
    void SceneManager::draw()
    {
        _scene->draw();
    }
    
    void SceneManager::recalculateProjection()
    {
        if (_scene)
        {
            _scene->recalculateProjection();
        }
        
    }
}
