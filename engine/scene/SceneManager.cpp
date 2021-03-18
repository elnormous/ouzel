// Ouzel by Elviss Strazdins

#include <cassert>
#include <algorithm>
#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Actor.hpp"

namespace ouzel::scene
{
    SceneManager::~SceneManager()
    {
        for (auto scene : scenes)
        {
            if (scene->entered) scene->leave();
            scene->sceneManger = nullptr;
        }
    }

    void SceneManager::setScene(Scene& scene)
    {
        if (scene.sceneManger) scene.sceneManger->removeScene(scene);

        scene.sceneManger = this;

        scenes.push_back(&scene);
    }

    bool SceneManager::removeScene(const Scene& scene)
    {
        bool result = false;

        const auto sceneIterator = std::find(scenes.begin(), scenes.end(), &scene);

        if (sceneIterator != scenes.end())
        {
            Scene* child = *sceneIterator;

            if (child->entered) child->leave();
            child->sceneManger = nullptr;
            scenes.erase(sceneIterator);

            result = true;
        }

        const auto ownedIterator = std::find_if(ownedScenes.begin(), ownedScenes.end(), [&scene](const auto& other) noexcept {
            return other.get() == &scene;
        });

        if (ownedIterator != ownedScenes.end())
            ownedScenes.erase(ownedIterator);

        return result;
    }

    void SceneManager::draw()
    {
        while (scenes.size() > 1)
            removeScene(*scenes.front());

        if (!scenes.empty())
        {
            Scene* scene = scenes.back();
            if (!scene->entered) scene->enter();
            scene->draw();
        }
    }
}
