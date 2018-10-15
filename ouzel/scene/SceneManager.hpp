// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>

namespace ouzel
{
    class Engine;

    namespace scene
    {
        class Scene;

        class SceneManager final
        {
            friend Engine;
        public:
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

        protected:
            SceneManager();

        private:
            std::vector<Scene*> scenes;
            std::vector<std::unique_ptr<Scene>> ownedScenes;
        };
    } // namespace scene
} // namespace ouzel
