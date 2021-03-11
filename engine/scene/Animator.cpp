// Ouzel by Elviss Strazdins

#include <algorithm>
#include <cassert>
#include "Animator.hpp"
#include "Actor.hpp"
#include "../core/Engine.hpp"

namespace ouzel::scene
{
    Animator::Animator(float initLength):
        length{initLength}
    {
        updateHandler.updateHandler = [this](const UpdateEvent& event) {
            update(event.delta);
            return false;
        };
    }

    Animator::~Animator()
    {
        if (parent) parent->removeAnimator(*this);

        for (const auto& animator : animators)
            animator->parent = nullptr;
    }

    void Animator::update(float delta)
    {
        if (running)
        {
            if (length == 0.0F) // never-ending animation
            {
                currentTime += delta;
                progress = 0.0F;
            }
            else if (currentTime + delta >= length)
            {
                done = true;
                running = false;
                progress = 1.0F;
                currentTime = length;

                auto finishEvent = std::make_unique<AnimationEvent>();
                finishEvent->type = Event::Type::animationFinish;
                finishEvent->component = this;
                engine->getEventDispatcher().dispatchEvent(std::move(finishEvent));
            }
            else
            {
                currentTime += delta;
                progress = currentTime / length;
            }

            updateProgress();
        }
        else
            updateHandler.remove();
    }

    void Animator::start()
    {
        engine->getEventDispatcher().addEventHandler(updateHandler);
        play();

        auto startEvent = std::make_unique<AnimationEvent>();
        startEvent->type = Event::Type::animationStart;
        startEvent->component = this;
        engine->getEventDispatcher().dispatchEvent(std::move(startEvent));
    }

    void Animator::play()
    {
        setProgress(0.0F);
        done = false;
        running = true;

        targetActor = actor;

        if (!targetActor && parent)
            targetActor = parent->getTargetActor();

        for (const auto& animator : animators)
            animator->play();
    }

    void Animator::resume()
    {
        running = true;
    }

    void Animator::stop(bool resetAnimation)
    {
        running = false;

        if (resetAnimation)
            reset();
    }

    void Animator::reset()
    {
        done = false;
        setProgress(0.0F);

        // reset animators in reverse order, so that Sequence is reset correctly
        for (auto i = animators.crbegin(); i != animators.crend(); ++i)
        {
            Animator* animator = *i;
            animator->reset();
        }
    }

    void Animator::setProgress(float newProgress)
    {
        progress = newProgress;
        currentTime = progress * length;

        updateProgress();
    }

    void Animator::addAnimator(std::unique_ptr<Animator> animator)
    {
        addAnimator(*animator);
        ownedAnimators.push_back(std::move(animator));
    }

    void Animator::addAnimator(Animator& animator)
    {
        if (animator.parent)
            animator.parent->removeAnimator(animator);

        animator.parent = this;

        animators.push_back(&animator);
    }

    bool Animator::removeAnimator(const Animator& animator)
    {
        bool result = false;

        const auto animatorIterator = std::find(animators.begin(), animators.end(), &animator);

        if (animatorIterator != animators.end())
        {
            Animator* child = *animatorIterator;

            child->parent = nullptr;
            animators.erase(animatorIterator);
            result = true;
        }

        const auto ownedAnimatorIterator = std::find_if(ownedAnimators.begin(), ownedAnimators.end(), [&animator](const auto& ownedAnimator) noexcept {
            return ownedAnimator.get() == &animator;
        });
        if (ownedAnimatorIterator != ownedAnimators.end())
            ownedAnimators.erase(ownedAnimatorIterator);

        return result;
    }

    void Animator::removeAllAnimators()
    {
        for (auto& animator : animators)
            animator->parent = nullptr;

        animators.clear();
        ownedAnimators.clear();
    }

    void Animator::removeFromParent()
    {
        if (parent) parent->removeAnimator(*this);
    }
}
