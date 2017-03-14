// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Parallel.h"

namespace ouzel
{
    namespace scene
    {
        Parallel::Parallel():
            Animator(0.0f)
        {

        }

        void Parallel::addAnimator(Animator* animator)
        {
            Animator::addAnimator(animator);

            if (animator->getLength() > length)
            {
                length = animator->getLength();
            }
        }

        bool Parallel::removeAnimator(Animator* animator)
        {
            if (Animator::removeAnimator(animator))
            {
                length = 0.0f;

                for (const auto& animator : animators)
                {
                    if (animator->getLength() > length)
                    {
                        length = animator->getLength();
                    }
                }

                return true;
            }

            return false;
        }

        void Parallel::updateProgress()
        {
            Animator::updateProgress();

            for (Animator* animator : animators)
            {
                float animationLength = animator->getLength();

                if (animationLength <= 0.0f || currentTime > animationLength)
                {
                    animator->setProgress(1.0f);
                }
                else
                {
                    animator->setProgress(currentTime / animationLength);
                }
            }
        }
    } // namespace scene
} // namespace ouzel
