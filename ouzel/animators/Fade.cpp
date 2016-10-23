// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Fade.h"
#include "scene/Node.h"

namespace ouzel
{
    namespace scene
    {
        Fade::Fade(float aLength, float aOpacity, bool aRelative):
            Animator(aLength), opacity(aOpacity), relative(aRelative)
        {
        }

        void Fade::start(Node* targetNode)
        {
            Animator::start(targetNode);

            if (targetNode)
            {
                startOpacity = targetNode->getOpacity();
                targetOpacity = relative ? startOpacity + opacity : opacity;

                diff = targetOpacity - startOpacity;
            }
        }

        void Fade::updateProgress()
        {
            Animator::updateProgress();

            if (node)
            {
                node->setOpacity(startOpacity + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
