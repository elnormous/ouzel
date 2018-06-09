// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Scale.hpp"
#include "scene/Actor.hpp"

namespace ouzel
{
    namespace scene
    {
        Scale::Scale(float initLength, const Vector3& initScale, bool initRelative):
            Animator(initLength), scale(initScale), relative(initRelative)
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
                targetActor->setScale(startScale + (diff * progress));
        }
    } // namespace scene
} // namespace ouzel
