// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_ANIMATOR_HPP
#define OUZEL_SCENE_ANIMATOR_HPP

#include <memory>
#include <vector>
#include "Component.hpp"
#include "../events/EventHandler.hpp"

namespace ouzel::scene
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

        auto isRunning() const noexcept { return running; }
        auto isDone() const noexcept { return done; }

        auto getLength() const noexcept { return length; }
        auto getCurrentTime() const noexcept { return currentTime; }

        auto getProgress() const noexcept { return progress; }
        virtual void setProgress(float newProgress);

        auto getTargetActor() const noexcept { return targetActor; }

        void addAnimator(std::unique_ptr<Animator> animator);
        void addAnimator(Animator& animator);
        bool removeAnimator(const Animator& animator);
        void removeAllAnimators();

        auto getParent() const noexcept { return parent; }
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
}

#endif // OUZEL_SCENE_ANIMATOR_HPP
