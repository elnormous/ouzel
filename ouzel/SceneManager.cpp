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
    
    void SceneManager::update(float delta)
    {
        _scene->update(delta);
    }
    
    void SceneManager::setScene(Scene* scene)
    {
        _scene = scene;
    }
    
    void SceneManager::draw()
    {
        _scene->draw();
    }
}
