// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
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

            void setScene(const std::shared_ptr<Scene>& newScene);
            const std::shared_ptr<Scene>& getScene() const { return scene; }
            void removeScene(const std::shared_ptr<Scene>& oldScene);

        protected:
            SceneManager();

            std::shared_ptr<Scene> scene = nullptr;
            std::shared_ptr<Scene> nextScene = nullptr;
        };
    } // namespace scene
} // namespace ouzel
