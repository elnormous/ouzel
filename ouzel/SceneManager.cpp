// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SceneManager.h"
#include "Scene.h"

namespace ouzel
{
    namespace scene
    {
        SceneManager::SceneManager()
        {
            
        }
        
        SceneManager::~SceneManager()
        {
            
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
            ++_locked;
        }
        
        void SceneManager::unlock()
        {
            if (--_locked == 0 && _newScene)
            {
                setScene(_newScene);
                _newScene.reset();
            }
        }
    } // namespace scene
} // namespace ouzel
