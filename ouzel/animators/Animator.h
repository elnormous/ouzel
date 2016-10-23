// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include "utils/Noncopyable.h"
#include "utils/Types.h"

namespace ouzel
{
    namespace scene
    {
        class Animator: public Noncopyable
        {
            friend Node;
        public:
            Animator(float aLength);
            virtual ~Animator();

            virtual void update(float delta);

            virtual void start(Node* targetNode);

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

        protected:
            void setNode(Node* newNode) { node = newNode; }

            virtual void updateProgress();

            float length = 0.0f;
            float currentTime = 0.0f;
            float progress = 0.0f;
            bool done = false;
            bool running = false;

            Node* node = nullptr;

            std::function<void()> finishHandler;
        };
    } // namespace scene
} // namespace ouzel
