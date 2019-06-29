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
            virtual ~Animator();

            virtual void update(float delta);

            virtual void start();
            virtual void play();

            virtual void resume();
            virtual void stop(bool resetAnimation = false);
            virtual void reset();

            inline bool isRunning() const { return running; }
            inline bool isDone() const { return done; }

            inline float getLength() const { return length; }
            inline float getCurrentTime() const { return currentTime; }

            inline float getProgress() const { return progress; }
            virtual void setProgress(float newProgress);

            inline Actor* getTargetActor() const { return targetActor; }

            void addAnimator(std::unique_ptr<Animator> animator);
            void addAnimator(Animator* animator);
            bool removeAnimator(Animator* animator);
            void removeAllAnimators();

            inline Animator* getParent() const { return parent; }
            void removeFromParent();

        protected:
            bool handleUpdate(const UpdateEvent& event);
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
