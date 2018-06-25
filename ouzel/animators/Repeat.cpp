// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "Repeat.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        Repeat::Repeat(Animator* animator, uint32_t initCount):
            Animator(animator->getLength() * static_cast<float>(initCount)), count(initCount)
        {
            addAnimator(animator);
        }

        Repeat::Repeat(const std::unique_ptr<Animator>& animator, uint32_t initCount):
            Repeat(animator.get(), initCount)
        {
        }

        void Repeat::reset()
        {
            Animator::reset();

            currentCount = 0;
        }

        void Repeat::updateProgress()
        {
            if (animators.empty()) return;

            if (animators.front()->getLength() != 0.0F)
            {
                currentCount = static_cast<uint32_t>(currentTime / animators.front()->getLength());

                if (count == 0 || currentCount < count)
                {
                    done = false;
                    running = true;

                    float remainingTime = currentTime - animators.front()->getLength() * static_cast<float>(currentCount);
                    animators.front()->setProgress(remainingTime / animators.front()->getLength());

                    Event resetEvent;
                    resetEvent.type = Event::Type::ANIMATION_RESET;
                    resetEvent.animationEvent.component = this;
                    engine->getEventDispatcher()->postEvent(resetEvent);
                }
                else
                {
                    done = true;
                    running = false;
                    currentTime = length;
                    progress = 1.0F;

                    Event finishEvent;
                    finishEvent.type = Event::Type::ANIMATION_FINISH;
                    finishEvent.animationEvent.component = this;
                    engine->getEventDispatcher()->postEvent(finishEvent);
                }
            }
        }
    } // namespace scene
} // namespace ouzel
