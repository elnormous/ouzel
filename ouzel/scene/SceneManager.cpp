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

        void SceneManager::setScene(const ScenePtr& newScene)
        {
            if (scene != newScene)
            {
                scene = newScene;

                if (scene)
                {
                    scene->recalculateProjection();
                }
            }
        }

        void SceneManager::draw()
        {
            if (scene)
            {
                scene->draw();
            }
        }

        void SceneManager::recalculateProjection()
        {
            if (scene)
            {
                scene->recalculateProjection();
            }
        }
    } // namespace scene
} // namespace ouzel
