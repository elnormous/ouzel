// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include <memory>
#include "scene/Component.hpp"
#include "scene/UpdateCallback.hpp"

namespace ouzel
{
    namespace scene
    {
        class Animator: public Component
        {
            friend Actor;
        public:
            explicit Animator(float initLength);
            virtual ~Animator();

            virtual void update(float delta);

            virtual void start();
            virtual void play();

            virtual void resume();
            virtual void stop(bool resetAnimation = false);
            virtual void reset();

            bool isRunning() const { return running; }
            bool isDone() const { return done; }

            inline float getLength() const { return length; }
            inline float getCurrentTime() const { return currentTime; }

            inline float getProgress() const { return progress; }
            virtual void setProgress(float newProgress);

            inline Actor* getTargetActor() const { return targetActor; }

            void addAnimator(Animator* animator)
            {
                addChildAnimator(animator);
            }

            template<class T> void addAnimator(const std::unique_ptr<T>& animator)
            {
                addChildAnimator(animator.get());
            }

            template<class T> void addAnimator(std::unique_ptr<T>&& animator)
            {
                addChildAnimator(animator.get());
                ownedAnimators.push_back(std::move(animator));
            }

            bool removeAnimator(Animator* animator)
            {
                return removeChildAnimator(animator);
            }

            template<class T> bool removeAnimator(const std::unique_ptr<T>& animator)
            {
                return removeChildAnimator(animator.get());
            }

            void removeAllAnimators();

            inline Animator* getParent() const { return parent; }
            void removeFromParent();

        protected:
            virtual void addChildAnimator(Animator* animator);
            virtual bool removeChildAnimator(Animator* animator);

            virtual void updateProgress() {}

            float length = 0.0F;
            float currentTime = 0.0F;
            float progress = 0.0F;
            bool done = false;
            bool running = false;

            Animator* parent = nullptr;
            Actor* targetActor = nullptr;

            UpdateCallback updateCallback;

            std::vector<Animator*> animators;
            std::vector<std::unique_ptr<Animator>> ownedAnimators;
        };
    } // namespace scene
} // namespace ouzel
