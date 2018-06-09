// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Fade.hpp"
#include "scene/Actor.hpp"

namespace ouzel
{
    namespace scene
    {
        Fade::Fade(float initLength, float initOpacity, bool initRelative):
            Animator(initLength), opacity(initOpacity), relative(initRelative)
        {
        }

        void Fade::play()
        {
            Animator::play();

            if (targetActor)
            {
                startOpacity = targetActor->getOpacity();
                targetOpacity = relative ? startOpacity + opacity : opacity;

                diff = targetOpacity - startOpacity;
            }
        }

        void Fade::updateProgress()
        {
            Animator::updateProgress();

            if (targetActor)
                targetActor->setOpacity(startOpacity + (diff * progress));
        }
    } // namespace scene
} // namespace ouzel
