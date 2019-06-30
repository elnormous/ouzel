// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cassert>
#include "Animator.hpp"
#include "Actor.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        Animator::Animator(float initLength):
            Component(Component::ANIMATOR), length(initLength)
        {
            updateHandler.updateHandler = std::bind(&Animator::handleUpdate, this, std::placeholders::_1);
        }

        Animator::~Animator()
        {
            if (parent) parent->removeAnimator(this);

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

                    std::unique_ptr<AnimationEvent> finishEvent = std::make_unique<AnimationEvent>();
                    finishEvent->type = Event::Type::ANIMATION_FINISH;
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

        bool Animator::handleUpdate(const UpdateEvent& event)
        {
            update(event.delta);
            return false;
        }

        void Animator::start()
        {
            engine->getEventDispatcher().addEventHandler(updateHandler);
            play();

            std::unique_ptr<AnimationEvent> startEvent = std::make_unique<AnimationEvent>();
            startEvent->type = Event::Type::ANIMATION_START;
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
            addAnimator(animator.get());
            ownedAnimators.push_back(std::move(animator));
        }

        void Animator::addAnimator(Animator* animator)
        {
            assert(animator);

            if (animator->parent)
                animator->parent->removeAnimator(animator);

            animator->parent = this;

            animators.push_back(animator);
        }

        bool Animator::removeAnimator(Animator* animator)
        {
            assert(animator);

            bool result = false;

            auto animatorIterator = std::find(animators.begin(), animators.end(), animator);

            if (animatorIterator != animators.end())
            {
                animator->parent = nullptr;
                animators.erase(animatorIterator);
                result = true;
            }

            auto ownedAnimatorIterator = std::find_if(ownedAnimators.begin(), ownedAnimators.end(), [animator](const std::unique_ptr<Animator>& ownedAnimator){
                return animator == ownedAnimator.get();
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
            if (parent) parent->removeAnimator(this);
        }
    } // namespace scene
} // namespace ouzel
