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
                if (locked)
                {
                    nextScene = scene;
                }
                else
                {
                    scene = newScene;

                    if (scene)
                    {
                        scene->recalculateProjection();
                    }
                }
            }
        }

        void SceneManager::draw()
        {
            if (scene)
            {
                lock();

                scene->draw();

                unlock();
            }
        }

        void SceneManager::recalculateProjection()
        {
            if (scene)
            {
                scene->recalculateProjection();
            }
        }

        void SceneManager::lock()
        {
            ++locked;
        }

        void SceneManager::unlock()
        {
            if (--locked == 0 && nextScene)
            {
                setScene(nextScene);
                nextScene.reset();
            }
        }
    } // namespace scene
} // namespace ouzel
