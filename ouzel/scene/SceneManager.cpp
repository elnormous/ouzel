// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include <cassert>
#include <algorithm>
#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Actor.hpp"
#include "thread/Lock.hpp"

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

            for (UpdateCallback* updateCallback : updateCallbackAddSet)
                updateCallback->sceneManager = nullptr;

            for (UpdateCallback* updateCallback : updateCallbacks)
            {
                auto i = std::find(updateCallbackDeleteSet.begin(), updateCallbackDeleteSet.end(), updateCallback);
                if (i == updateCallbackDeleteSet.end()) updateCallback->sceneManager = nullptr;
            }
        }

        void SceneManager::update()
        {
            executeAllOnUpdateThread();

            std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
            auto diff = currentTime - previousUpdateTime;

            if (diff > std::chrono::milliseconds(1)) // at least one millisecond has passed
            {
                if (diff > std::chrono::seconds(1000 / 20)) diff = std::chrono::milliseconds(1000 / 20); // limit the update rate to a minimum 20 FPS

                previousUpdateTime = currentTime;
                float delta = std::chrono::duration_cast<std::chrono::microseconds>(diff).count() / 1000000.0F;

                for (UpdateCallback* updateCallback : updateCallbackDeleteSet)
                {
                    auto i = std::find(updateCallbacks.begin(), updateCallbacks.end(), updateCallback);

                    if (i != updateCallbacks.end())
                        updateCallbacks.erase(i);
                }

                updateCallbackDeleteSet.clear();

                for (UpdateCallback* updateCallback : updateCallbackAddSet)
                {
                    auto i = std::find(updateCallbacks.begin(), updateCallbacks.end(), updateCallback);

                    if (i == updateCallbacks.end())
                    {
                        auto upperBound = std::upper_bound(updateCallbacks.begin(), updateCallbacks.end(), updateCallback,
                                                           [](const UpdateCallback* a, const UpdateCallback* b) {
                                                               return a->priority > b->priority;
                                                           });

                        updateCallbacks.insert(upperBound, updateCallback);
                    }
                }

                updateCallbackAddSet.clear();

                for (UpdateCallback* updateCallback : updateCallbacks)
                {
                    auto i = std::find(updateCallbackDeleteSet.begin(), updateCallbackDeleteSet.end(), updateCallback);

                    if (i == updateCallbackDeleteSet.end())
                    {
                        updateCallback->timeSinceLastUpdate += delta;

                        if (updateCallback->timeSinceLastUpdate >= updateCallback->interval)
                        {
                            updateCallback->timeSinceLastUpdate = (updateCallback->interval > 0.0F) ? fmodf(updateCallback->timeSinceLastUpdate, updateCallback->interval) : 0.0F;
                            if (updateCallback->callback) updateCallback->callback(delta);
                        }
                    }
                }
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

        void SceneManager::scheduleUpdate(UpdateCallback* callback)
        {
            if (callback->sceneManager)
                callback->sceneManager->unscheduleUpdate(callback);

            callback->sceneManager = this;

            updateCallbackAddSet.insert(callback);

            auto setIterator = updateCallbackDeleteSet.find(callback);

            if (setIterator != updateCallbackDeleteSet.end())
                updateCallbackDeleteSet.erase(setIterator);
        }

        void SceneManager::unscheduleUpdate(UpdateCallback* callback)
        {
            if (callback->sceneManager == this)
                callback->sceneManager = nullptr;

            updateCallbackDeleteSet.insert(callback);

            auto setIterator = updateCallbackAddSet.find(callback);

            if (setIterator != updateCallbackAddSet.end())
                updateCallbackAddSet.erase(setIterator);
        }

        void SceneManager::executeOnUpdateThread(const std::function<void(void)>& func)
        {
            Lock lock(updateThreadExecuteMutex);

            updateThreadExecuteQueue.push(func);
        }

        void SceneManager::executeAllOnUpdateThread()
        {
            std::function<void(void)> func;

            for (;;)
            {
                {
                    Lock lock(updateThreadExecuteMutex);
                    if (updateThreadExecuteQueue.empty()) break;

                    func = std::move(updateThreadExecuteQueue.front());
                    updateThreadExecuteQueue.pop();
                }

                if (func) func();

            }
        }
    } // namespace scene
} // namespace ouzel
