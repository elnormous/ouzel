// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Repeat.h"

namespace ouzel
{
    namespace scene
    {
        Repeat::Repeat(const AnimatorPtr& pAnimator, uint32_t pCount):
            Animator(pAnimator->getLength() * static_cast<float>(pCount)), animator(pAnimator), count(pCount)
        {
        }

        void Repeat::start(const NodePtr& targetNode)
        {
            Animator::start(targetNode);

            if (animator)
            {
                animator->start(targetNode);
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
            if (!animator || animator->getLength() <= 0.0f)
            {
                currentCount = count;
                done = true;
                running = false;
                currentTime = length;
                progress = 1.0f;
            }
            else
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
                }
            }
        }
    } // namespace scene
} // namespace ouzel
