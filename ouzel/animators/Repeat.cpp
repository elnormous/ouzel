// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

                    std::unique_ptr<AnimationEvent> resetEvent(new AnimationEvent());
                    resetEvent->type = Event::Type::ANIMATION_RESET;
                    resetEvent->component = this;
                    engine->getEventDispatcher().dispatchEvent(std::move(resetEvent));
                }
                else
                {
                    done = true;
                    running = false;
                    currentTime = length;
                    progress = 1.0F;

                    std::unique_ptr<AnimationEvent> finishEvent(new AnimationEvent());
                    finishEvent->type = Event::Type::ANIMATION_FINISH;
                    finishEvent->component = this;
                    engine->getEventDispatcher().dispatchEvent(std::move(finishEvent));
                }
            }
        }
    } // namespace scene
} // namespace ouzel
