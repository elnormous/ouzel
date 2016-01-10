// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SceneManager.h"
#include "Scene.h"

namespace ouzel
{
    SceneManager::SceneManager()
    {
        
    }
    
    SceneManager::~SceneManager()
    {
        
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
