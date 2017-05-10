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

        void SceneManager::setScene(Scene* scene)
        {
            if (scene)
            {
                if (scene->sceneManger) scene->sceneManger->removeScene(scene);

                scene->sceneManger = this;

                scenes.push_back(scene);
            }
        }

        void SceneManager::setScene(std::unique_ptr<Scene>&& scene)
        {
            setScene(scene.get());
            ownedScenes.push_back(std::forward<std::unique_ptr<Scene>>(scene));
        }

        bool SceneManager::removeScene(Scene* scene)
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
