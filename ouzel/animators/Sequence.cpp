// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <numeric>
#include "Sequence.h"

namespace ouzel
{
    namespace scene
    {
        Sequence::Sequence(const std::vector<AnimatorPtr>& pAnimators):
            Animator(std::accumulate(pAnimators.begin(), pAnimators.end(), 0.0f, [](float a, const AnimatorPtr& b) { return a + b->getLength(); })), animators(pAnimators)
        {
        }

        void Sequence::start(const NodePtr& node)
        {
            Animator::start(node);

            for (const auto& animator : animators)
            {
                animator->start(node);
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
