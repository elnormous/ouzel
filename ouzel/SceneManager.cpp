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
            lock();
            
            _scene->update(delta);
            
            unlock();
        }
    }
    
    void SceneManager::setScene(const ScenePtr& scene)
    {
        if (scene != _scene)
        {
            if (_locked)
            {
                _newScene = scene;
            }
            else
            {
                _scene = scene;
                
                if (_scene)
                {
                    _scene->recalculateProjection();
                }
            }
        }
    }
    
    void SceneManager::draw()
    {
        if (_scene)
        {
            lock();
            
            _scene->draw();
            
            unlock();
        }
    }
    
    void SceneManager::recalculateProjection()
    {
        if (_scene)
        {
            _scene->recalculateProjection();
        }
    }
    
    void SceneManager::lock()
    {
        _locked = true;
    }
    
    void SceneManager::unlock()
    {
        _locked = false;
        
        if (_newScene)
        {
            setScene(_newScene);
            _newScene.reset();
        }
    }
}
