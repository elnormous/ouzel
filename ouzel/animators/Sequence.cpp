// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <numeric>
#include "Sequence.h"

namespace ouzel
{
    namespace scene
    {
        Sequence::Sequence(const std::vector<Animator*>& aAnimators):
            Animator(std::accumulate(aAnimators.begin(), aAnimators.end(), 0.0f, [](float a, Animator* b) { return a + b->getLength(); })), animators(aAnimators)
        {
        }

        void Sequence::start(Node* newTargetNode)
        {
            Animator::start(newTargetNode);

            for (const auto& animator : animators)
            {
                animator->start(targetNode);
            }
        }

        void Sequence::reset()
        {
            Animator::reset();

            for (const auto& animator : animators)
            {
                animator->reset();
            }
        }

        void Sequence::removeAnimator(Animator* oldAnimator)
        {
            Animator::removeAnimator(oldAnimator);

            auto i = std::find(animators.begin(), animators.end(), oldAnimator);

            if (i != animators.end())
            {
                animators.erase(i);
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
                    animator->setProgress((currentTime - time) / animator->getLength());
                }

                time += animator->getLength();
            }
        }
    } // namespace scene
} // namespace ouzel
