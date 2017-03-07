// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <functional>
#include "scene/Component.h"
#include "core/UpdateCallback.h"

namespace ouzel
{
    namespace scene
    {
        class Node;

        class Animator: public Component, public std::enable_shared_from_this<Animator>
        {
            friend Node;
        public:
            Animator(float aLength);

            virtual void update(float delta);

            virtual void start();
            virtual void play();

            virtual void resume();
            virtual void stop(bool resetAnimation = false);
            virtual void reset();

            bool isRunning() const { return running; }
            bool isDone() const { return done; }

            float getLength() const { return length; }
            float getCurrentTime() const { return currentTime; }

            float getProgress() const { return progress; }
            virtual void setProgress(float newProgress);

            void setFinishHandler(const std::function<void()>& handler) { finishHandler = handler; }

            void addAnimator(const std::shared_ptr<Animator>& animator);
            void removeFromParent();
            virtual void removeAnimator(const std::shared_ptr<Animator>& animator);

        protected:
            virtual void updateProgress();

            float length = 0.0f;
            float currentTime = 0.0f;
            float progress = 0.0f;
            bool done = false;
            bool running = false;

            std::weak_ptr<Animator> parent;
            std::weak_ptr<Node> targetNode;

            std::function<void()> finishHandler;

            UpdateCallback updateCallback;
        };
    } // namespace scene
} // namespace ouzel
