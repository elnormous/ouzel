// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SceneManager.h"
#include "Scene.h"
#include "core/Engine.h"
#include "Node.h"

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

        void SceneManager::setScene(const std::shared_ptr<Scene>& newScene)
        {
            if (scene != newScene)
            {
                nextScene = newScene;
            }
        }

        void SceneManager::removeScene(const std::shared_ptr<Scene>& oldScene)
        {
            if (scene == oldScene) scene = nullptr;
            if (nextScene == oldScene) nextScene = nullptr;
        }

        void SceneManager::draw()
        {
            if (nextScene)
            {
                if (scene)
                {
                    scene->leave();
                }

                scene = nextScene;
                nextScene = nullptr;

                if (scene)
                {
                    scene->enter();
                }
            }

            if (scene)
            {
                scene->draw();
            }
        }
    } // namespace scene
} // namespace ouzel
