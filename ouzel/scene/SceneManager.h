// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <cstdint>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "math/Vector2.h"
#include "math/Rectangle.h"

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

            ScenePtr scene;
        };
    } // namespace scene
} // namespace ouzel
