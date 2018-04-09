// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <numeric>
#include "Sequence.hpp"

namespace ouzel
{
    namespace scene
    {
        Sequence::Sequence(const std::vector<Animator*>& initAnimators):
            Animator(std::accumulate(initAnimators.begin(), initAnimators.end(), 0.0F, [](float a, Animator* b) { return a + b->getLength(); }))
        {
            for (Animator* animator : initAnimators)
            {
                addAnimator(animator);
            }
        }

        Sequence::Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators):
            Animator(std::accumulate(initAnimators.begin(), initAnimators.end(), 0.0F, [](float a, const std::unique_ptr<Animator>& b) { return a + b->getLength(); }))
        {
            for (const std::unique_ptr<Animator>& animator : initAnimators)
            {
                addAnimator(animator.get());
            }
        }

        void Sequence::play()
        {
            setProgress(0.0F);
            done = false;
            running = true;

            targetActor = actor;

            if (!targetActor && parent)
            {
                targetActor = parent->getTargetActor();
            }

            if (!animators.empty())
            {
                currentAnimator = animators.front();
                currentAnimator->play();
            }
            else
            {
                currentAnimator = nullptr;
            }
        }

        void Sequence::updateProgress()
        {
            Animator::updateProgress();

            float time = 0.0F;

            for (Animator* animator : animators)
            {
                if (animator->getLength() <= 0.0F || currentTime > time + animator->getLength())
                {
                    if (animator == currentAnimator) animator->setProgress(1.0F);
                }
                else if (currentTime <= time)
                {
                    if (animator == currentAnimator) animator->setProgress(0.0F);
                }
                else
                {
                    if (currentAnimator != animator)
                    {
                        currentAnimator = animator;
                        animator->play();
                    }

                    animator->setProgress((currentTime - time) / animator->getLength());
                }

                time += animator->getLength();
            }
        }
    } // namespace scene
} // namespace ouzel
