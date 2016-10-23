// Copyright (C) 2016 Elviss Strazdins
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

        void SceneManager::setScene(Scene* newScene)
        {
            if (scene != newScene)
            {
                nextScene = newScene;
            }
        }

        void SceneManager::removeScene(Scene* oldScene)
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

                scene = std::move(nextScene);

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
