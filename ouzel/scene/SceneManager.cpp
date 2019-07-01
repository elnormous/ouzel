// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cassert>
#include <algorithm>
#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Actor.hpp"

namespace ouzel
{
    namespace scene
    {
        SceneManager::~SceneManager()
        {
            for (Scene* scene : scenes)
            {
                if (scene->entered) scene->leave();
                scene->sceneManger = nullptr;
            }
        }

        void SceneManager::setScene(Scene* scene)
        {
            assert(scene);

            if (scene->sceneManger) scene->sceneManger->removeScene(scene);

            scene->sceneManger = this;

            scenes.push_back(scene);
        }

        bool SceneManager::removeScene(Scene* scene)
        {
            bool result = false;

            auto sceneIterator = std::find(scenes.begin(), scenes.end(), scene);

            if (sceneIterator != scenes.end())
            {
                if (scene->entered) scene->leave();
                scene->sceneManger = nullptr;
                scenes.erase(sceneIterator);

                result = true;
            }

            auto ownedIterator = std::find_if(ownedScenes.begin(), ownedScenes.end(), [scene](const auto& other) {
                return other.get() == scene;
            });

            if (ownedIterator != ownedScenes.end())
                ownedScenes.erase(ownedIterator);

            return result;
        }

        void SceneManager::draw()
        {
            while (scenes.size() > 1)
                removeScene(scenes.front());

            if (!scenes.empty())
            {
                Scene* scene = scenes.back();
                if (!scene->entered) scene->enter();
                scene->draw();
            }
        }
    } // namespace scene
} // namespace ouzel
