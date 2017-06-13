// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Repeat.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace scene
    {
        Repeat::Repeat(Animator* animator, uint32_t aCount):
            Animator(animator->getLength() * static_cast<float>(aCount)), count(aCount)
        {
            addAnimator(animator);
        }

        Repeat::Repeat(const std::unique_ptr<Animator>& animator, uint32_t aCount):
            Repeat(animator.get(), aCount)
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

            if (animators[0]->getLength() != 0.0f)
            {
                currentCount = static_cast<uint32_t>(currentTime / animators[0]->getLength());

                if (count == 0 || currentCount < count)
                {
                    done = false;
                    running = true;

                    float remainingTime = currentTime - animators[0]->getLength() * static_cast<float>(currentCount);
                    animators[0]->setProgress(remainingTime / animators[0]->getLength());

                    Event resetEvent;
                    resetEvent.type = Event::Type::RESET;
                    resetEvent.animationEvent.component = this;
                    sharedEngine->getEventDispatcher()->postEvent(resetEvent);
                }
                else
                {
                    done = true;
                    running = false;
                    currentTime = length;
                    progress = 1.0f;

                    Event finishEvent;
                    finishEvent.type = Event::Type::FINISH;
                    finishEvent.animationEvent.component = this;
                    sharedEngine->getEventDispatcher()->postEvent(finishEvent);
                }
            }
        }
    } // namespace scene
} // namespace ouzel
