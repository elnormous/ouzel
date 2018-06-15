// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <queue>
#include <set>
#include <vector>
#include "thread/Mutex.hpp"

namespace ouzel
{
    class Engine;

    namespace scene
    {
        class Scene;
        class UpdateCallback;

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

            void executeOnUpdateThread(const std::function<void(void)>& func);

            void scheduleUpdate(UpdateCallback* callback);
            void unscheduleUpdate(UpdateCallback* callback);

        protected:
            SceneManager();

        private:
            void update();
            void executeAllOnUpdateThread();

            std::vector<Scene*> scenes;
            std::vector<std::unique_ptr<Scene>> ownedScenes;

            std::chrono::steady_clock::time_point previousUpdateTime;

            std::vector<UpdateCallback*> updateCallbacks;
            std::set<UpdateCallback*> updateCallbackAddSet;
            std::set<UpdateCallback*> updateCallbackDeleteSet;

            std::queue<std::function<void(void)>> updateThreadExecuteQueue;
            Mutex updateThreadExecuteMutex;
        };
    } // namespace scene
} // namespace ouzel
