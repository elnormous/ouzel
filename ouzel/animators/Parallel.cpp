// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Parallel.h"

namespace ouzel
{
    namespace scene
    {
        Parallel::Parallel(const std::vector<AnimatorPtr>& aAnimators):
            Animator(0.0f), animators(aAnimators)
        {
            for (const auto& animator : animators)
            {
                if (animator->getLength() > length)
                {
                    length = animator->getLength();
                }
            }
        }

        void Parallel::start(Node* targetNode)
        {
            Animator::start(targetNode);

            for (const auto& animator : animators)
            {
                animator->start(targetNode);
            }
        }

        void Parallel::reset()
        {
            Animator::reset();

            for (const auto& animator : animators)
            {
                animator->reset();
            }
        }

        void Parallel::updateProgress()
        {
            Animator::updateProgress();

            for (const auto& animator : animators)
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
