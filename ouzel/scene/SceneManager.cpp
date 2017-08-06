// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "SceneManager.hpp"
#include "Scene.hpp"
#include "core/Engine.hpp"
#include "Node.hpp"

namespace ouzel
{
    namespace scene
    {
        SceneManager::SceneManager()
        {
        }

        SceneManager::~SceneManager()
        {
            for (Scene* scene : scenes)
            {
                if (scene->entered) scene->leave();
                scene->sceneManger = nullptr;
            }
        }

        void SceneManager::addChildScene(Scene* scene)
        {
            if (scene)
            {
                if (scene->sceneManger) scene->sceneManger->removeScene(scene);

                scene->sceneManger = this;

                scenes.push_back(scene);
            }
        }

        bool SceneManager::removeChildScene(Scene* scene)
        {
            bool result = false;

            std::vector<Scene*>::iterator sceneIterator = std::find(scenes.begin(), scenes.end(), scene);

            if (sceneIterator != scenes.end())
            {
                if (scene->entered) scene->leave();
                scene->sceneManger = nullptr;
                scenes.erase(sceneIterator);

                result = true;
            }

            std::vector<std::unique_ptr<Scene>>::iterator ownedIterator = std::find_if(ownedScenes.begin(), ownedScenes.end(), [scene](const std::unique_ptr<Scene>& other) {
                return other.get() == scene;
            });

            if (ownedIterator != ownedScenes.end())
            {
                ownedScenes.erase(ownedIterator);
            }

            return result;
        }

        void SceneManager::draw()
        {
            while (scenes.size() > 1)
            {
                removeScene(scenes.front());
            }

            if (!scenes.empty())
            {
                Scene* scene = scenes.back();
                if (!scene->entered) scene->enter();
                scene->draw();
            }
        }
    } // namespace scene
} // namespace ouzel
