// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_ANIMATOR_HPP
#define OUZEL_SCENE_ANIMATOR_HPP

#include <memory>
#include <vector>
#include "scene/Component.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace scene
    {
        class Animator: public Component
        {
            friend Actor;
        public:
            explicit Animator(float initLength);
            ~Animator() override;

            virtual void update(float delta);

            virtual void start();
            virtual void play();

            virtual void resume();
            virtual void stop(bool resetAnimation = false);
            virtual void reset();

            inline auto isRunning() const noexcept { return running; }
            inline auto isDone() const noexcept { return done; }

            inline auto getLength() const noexcept { return length; }
            inline auto getCurrentTime() const noexcept { return currentTime; }

            inline auto getProgress() const noexcept { return progress; }
            virtual void setProgress(float newProgress);

            inline auto getTargetActor() const noexcept { return targetActor; }

            void addAnimator(std::unique_ptr<Animator> animator);
            void addAnimator(Animator* animator);
            bool removeAnimator(const Animator* animator);
            void removeAllAnimators();

            inline auto getParent() const noexcept { return parent; }
            void removeFromParent();

        protected:
            virtual void updateProgress() {}

            float length = 0.0F;
            float currentTime = 0.0F;
            float progress = 0.0F;
            bool done = false;
            bool running = false;

            Animator* parent = nullptr;
            Actor* targetActor = nullptr;

            EventHandler updateHandler;

            std::vector<Animator*> animators;
            std::vector<std::unique_ptr<Animator>> ownedAnimators;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_ANIMATOR_HPP
