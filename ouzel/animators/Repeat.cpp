// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Repeat.h"

namespace ouzel
{
    namespace scene
    {
        Repeat::Repeat(const std::shared_ptr<Animator>& aAnimator, uint32_t aCount):
            Animator(aAnimator->getLength() * static_cast<float>(aCount)), animator(aAnimator), count(aCount)
        {
            addAnimator(animator);
        }

        void Repeat::play()
        {
            Animator::play();

            if (animator)
            {
                animator->play();
            }
        }

        void Repeat::reset()
        {
            Animator::reset();

            if (animator)
            {
                animator->reset();
            }

            currentCount = 0;
        }

        void Repeat::updateProgress()
        {
            if (!animator) return;

            if (animator->getLength() != 0.0f)
            {
                currentCount = static_cast<uint32_t>(currentTime / animator->getLength());

                if (count == 0 || currentCount < count)
                {
                    done = false;
                    running = true;

                    float remainingTime = currentTime - animator->getLength() * static_cast<float>(currentCount);
                    animator->setProgress(remainingTime / animator->getLength());
                }
                else
                {
                    done = true;
                    running = false;
                    currentTime = length;
                    progress = 1.0f;
                    if (finishHandler) finishHandler();
                }
            }
        }
    } // namespace scene
} // namespace ouzel
