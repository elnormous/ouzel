#ifndef FLUXION_SCENE_ANIMATOR_HPP
#define FLUXION_SCENE_ANIMATOR_HPP

#include <memory>
#include <vector>
#include "Component.hpp"
#include "../events/EventHandler.hpp"

namespace fluxion::scene
{
    class Animator: public Component
    {
        friend Actor;
    public:
        // Default-construct an empty animator (length=0). Useful as
        // the chained-from base for default-constructible subclasses
        // (Move / Rotate / etc.) that the editor's TypeRegistry
        // creates when the user clicks "Add Component". The named
        // constructor that takes a length stays the primary entry
        // point for code that knows the duration up front.
        Animator() = default;

        explicit Animator(float initLength);
        ~Animator() override;

        virtual void update(float delta);

        void start();
        virtual void play();

        void resume();
        void stop(bool resetAnimation = false);
        virtual void reset();

        auto isRunning() const noexcept { return running; }
        auto isDone() const noexcept { return done; }

        auto getLength() const noexcept { return length; }
        // Editor-friendly setter so the Inspector can tweak duration
        // without forcing callers to rebuild the animator instance.
        // Has no effect on a currently-running playback until the
        // next play() call.
        void setLength(float newLength) noexcept { length = newLength; }

        auto getCurrentTime() const noexcept { return currentTime; }

        auto getProgress() const noexcept { return progress; }
        void setProgress(float newProgress);

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

#endif // FLUXION_SCENE_ANIMATOR_HPP
