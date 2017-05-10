// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include "utils/Noncopyable.h"

namespace ouzel
{
    class Engine;

    namespace scene
    {
        class Scene;

        class SceneManager: public Noncopyable
        {
            friend Engine;
        public:
            virtual ~SceneManager();

            void draw();

            void setScene(Scene* scene);
            void setScene(std::unique_ptr<Scene>&& scene);

            bool removeScene(Scene* scene);
            Scene* getScene() const { return scenes.empty() ? nullptr : scenes.back(); }

        protected:
            SceneManager();

            std::vector<Scene*> scenes;
            std::vector<std::unique_ptr<Scene>> ownedScenes;
        };
    } // namespace scene
} // namespace ouzel
