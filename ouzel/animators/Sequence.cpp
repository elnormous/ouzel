// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <numeric>
#include "Sequence.hpp"

namespace ouzel
{
    namespace scene
    {
        Sequence::Sequence(const std::vector<Animator*>& aAnimators):
            Animator(std::accumulate(aAnimators.begin(), aAnimators.end(), 0.0f, [](float a, Animator* b) { return a + b->getLength(); }))
        {
            for (Animator* animator : aAnimators)
            {
                addAnimator(animator);
            }
        }

        Sequence::Sequence(const std::vector<std::unique_ptr<Animator>>& aAnimators):
            Animator(std::accumulate(aAnimators.begin(), aAnimators.end(), 0.0f, [](float a, const std::unique_ptr<Animator>& b) { return a + b->getLength(); }))
        {
            for (const std::unique_ptr<Animator>& animator : aAnimators)
            {
                addAnimator(animator.get());
            }
        }

        void Sequence::play()
        {
            setProgress(0.0f);
            done = false;
            running = true;

            targetNode = node;

            if (!targetNode)
            {
                if (parent)
                {
                    targetNode = parent->getTargetNode();
                }
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

            float time = 0.0f;

            for (Animator* animator : animators)
            {
                if (animator->getLength() <= 0.0f || currentTime > time + animator->getLength())
                {
                    if (animator == currentAnimator) animator->setProgress(1.0f);
                }
                else if (currentTime <= time)
                {
                    if (animator == currentAnimator) animator->setProgress(0.0f);
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
