// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sequence.h"

namespace ouzel
{
    namespace scene
    {
        Sequence::Sequence():
            Animator(0.0f)
        {
        }

        void Sequence::addAnimator(Animator* animator)
        {
            Animator::addAnimator(animator);

            length = 0.0f;

            for (const auto& animator : animators)
            {
                length += animator->getLength();
            }
        }

        bool Sequence::removeAnimator(Animator* animator)
        {
            if (Animator::removeAnimator(animator))
            {
                length = 0.0f;

                for (const auto& animator : animators)
                {
                    length += animator->getLength();
                }

                return true;
            }
            
            return false;
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
