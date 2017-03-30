// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <numeric>
#include "Sequence.h"

namespace ouzel
{
    namespace scene
    {
        Sequence::Sequence(const std::vector<std::shared_ptr<Animator>>& aAnimators):
            Animator(std::accumulate(aAnimators.begin(), aAnimators.end(), 0.0f, [](float a, const std::shared_ptr<Animator>& b) { return a + b->getLength(); }))
        {
            for (const auto& animator : aAnimators)
            {
                addAnimator(animator);
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
                currentAnimator.reset();
            }
        }

        void Sequence::updateProgress()
        {
            Animator::updateProgress();

            float time = 0.0f;

            for (const auto& animator : animators)
            {
                if (animator->getLength() <= 0.0f || currentTime > time + animator->getLength())
                {
                    animator->setProgress(1.0f);
                }
                else if (currentTime <= time)
                {
                    animator->setProgress(0.0f);
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
