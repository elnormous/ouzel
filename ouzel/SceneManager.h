// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <cstdint>
#include "Types.h"
#include "Noncopyable.h"
#include "Vector2.h"
#include "Rectangle.h"

namespace ouzel
{
    class Engine;
    
    namespace scene
    {
        class SceneManager: public Noncopyable
        {
            friend Engine;
        public:
            virtual ~SceneManager();

            void draw();

            void setScene(const ScenePtr& newScene);
            const ScenePtr& getScene() const { return scene; }

            virtual void recalculateProjection();

        protected:
            SceneManager();

            void lock();
            void unlock();

            ScenePtr scene;

            ScenePtr nextScene;
            int32_t locked = 0;
        };
    } // namespace scene
} // namespace ouzel
