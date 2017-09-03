// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Scale.hpp"
#include "scene/Actor.hpp"

namespace ouzel
{
    namespace scene
    {
        Scale::Scale(float aLength, const Vector3& aScale, bool aRelative):
            Animator(aLength), scale(aScale), relative(aRelative)
        {
        }

        void Scale::play()
        {
            Animator::play();

            if (targetActor)
            {
                startScale = targetActor->getScale();
                targetScale = relative ? startScale + scale : scale;

                diff = targetScale - startScale;
            }
        }

        void Scale::updateProgress()
        {
            Animator::updateProgress();

            if (targetActor)
            {
                targetActor->setScale(startScale + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
