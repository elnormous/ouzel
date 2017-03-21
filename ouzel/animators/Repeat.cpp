// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Repeat.h"

namespace ouzel
{
    namespace scene
    {
        Repeat::Repeat(const std::shared_ptr<Animator>& animator, uint32_t aCount):
            Animator(animator->getLength() * static_cast<float>(aCount)), count(aCount)
        {
            addAnimator(animator);
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
