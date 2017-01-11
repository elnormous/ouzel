// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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

            void setScene(Scene* newScene);
            const Scene* getScene() const { return scene; }
            void removeScene(Scene* oldScene);

        protected:
            SceneManager();

            Scene* scene = nullptr;
            Scene* nextScene = nullptr;
        };
    } // namespace scene
} // namespace ouzel
