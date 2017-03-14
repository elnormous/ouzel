// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Repeat.h"

namespace ouzel
{
    namespace scene
    {
        Repeat::Repeat(uint32_t aCount):
            Animator(0.0f), count(aCount)
        {
        }

        void Repeat::addAnimator(Animator* animator)
        {
            Animator::addAnimator(animator);

            if (animators.empty())
            {
                length = 0.0f;
            }
            else
            {
                length = animators[0]->getLength() * static_cast<float>(count);
            }
        }

        bool Repeat::removeAnimator(Animator* animator)
        {
            if (Animator::removeAnimator(animator))
            {
                if (animators.empty())
                {
                    length = 0.0f;
                }
                else
                {
                    length = animators[0]->getLength() * static_cast<float>(count);
                }

                return true;
            }
            
            return false;
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
