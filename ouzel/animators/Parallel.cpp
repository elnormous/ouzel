// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Parallel.hpp"

namespace ouzel
{
    namespace scene
    {
        Parallel::Parallel(const std::vector<Animator*>& aAnimators):
            Animator(0.0f)
        {
            for (Animator* animator : aAnimators)
            {
                addAnimator(animator);

                if (animator->getLength() > length)
                {
                    length = animator->getLength();
                }
            }
        }

        Parallel::Parallel(const std::vector<std::unique_ptr<Animator>>& aAnimators):
            Animator(0.0f)
        {
            for (const std::unique_ptr<Animator>& animator : aAnimators)
            {
                addAnimator(animator.get());

                if (animator->getLength() > length)
                {
                    length = animator->getLength();
                }
            }
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
