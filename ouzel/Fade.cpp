// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Fade.h"
#include "Node.h"

namespace ouzel
{
    namespace scene
    {
        Fade::Fade(float pLength, float pOpacity, bool pRelative):
            Animator(pLength), opacity(pOpacity), relative(pRelative)
        {

        }

        void Fade::start(const NodePtr& targetNode)
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

            if (NodePtr targetNode = node.lock())
            {
                targetNode->setOpacity(startOpacity + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
