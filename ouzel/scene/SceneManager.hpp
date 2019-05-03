// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_SCENEMANAGER_HPP
#define OUZEL_SCENE_SCENEMANAGER_HPP

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>

namespace ouzel
{
    namespace scene
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

            void setScene(Scene* scene);

            template<typename T> void setScene(const std::unique_ptr<T>& scene)
            {
                setScene(scene.get());
            }

            template<typename T> void setScene(std::unique_ptr<T>&& scene)
            {
                setScene(scene.get());
                ownedScenes.push_back(std::move(scene));
            }

            bool removeScene(Scene* scene);

            template<typename T> bool setScene(const std::unique_ptr<T>& scene)
            {
                return removeScene(scene.get());
            }

            inline Scene* getScene() const { return scenes.empty() ? nullptr : scenes.back(); }

        private:
            std::vector<Scene*> scenes;
            std::vector<std::unique_ptr<Scene>> ownedScenes;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_SCENEMANAGER_HPP
