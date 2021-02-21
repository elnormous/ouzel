// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_SCENEMANAGER_HPP
#define OUZEL_SCENE_SCENEMANAGER_HPP

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>

namespace ouzel::scene
{
    class Scene;

    class SceneManager final
    {
    public:
        SceneManager() = default;
        ~SceneManager();

        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;

        void draw();

        void setScene(Scene& scene);

        template <class T> void setScene(std::unique_ptr<T> scene)
        {
            setScene(*scene);
            ownedScenes.push_back(std::move(scene));
        }

        bool removeScene(const Scene& scene);

        auto getScene() const noexcept { return scenes.empty() ? nullptr : scenes.back(); }

    private:
        std::vector<Scene*> scenes;
        std::vector<std::unique_ptr<Scene>> ownedScenes;
    };
}

#endif // OUZEL_SCENE_SCENEMANAGER_HPP
