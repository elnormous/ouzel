// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "utils/Memory.hpp"

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
            virtual ~SceneManager();

            SceneManager(const SceneManager&) = delete;
            SceneManager& operator=(const SceneManager&) = delete;

            SceneManager(SceneManager&&) = delete;
            SceneManager& operator=(SceneManager&&) = delete;

            void draw();

            void setScene(Scene* scene)
            {
                addChildScene(scene);
            }
            template<class T> void setScene(const UniquePtr<T>& scene)
            {
                addChildScene(scene.get());
            }
            template<class T> void setScene(UniquePtr<T>&& scene)
            {
                addChildScene(scene.get());
                ownedScenes.push_back(std::move(scene));
            }

            bool removeScene(Scene* scene)
            {
                return removeChildScene(scene);
            }
            template<class T> bool setScene(const UniquePtr<T>& scene)
            {
                return removeChildScene(scene.get());
            }
            inline Scene* getScene() const { return scenes.empty() ? nullptr : scenes.back(); }

        protected:
            SceneManager();
            virtual void addChildScene(Scene* scene);
            virtual bool removeChildScene(Scene* scene);

            std::vector<Scene*> scenes;
            std::vector<UniquePtr<Scene>> ownedScenes;
        };
    } // namespace scene
} // namespace ouzel
