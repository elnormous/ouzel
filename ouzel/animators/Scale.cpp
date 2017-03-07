// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Scale.h"
#include "scene/Node.h"

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

            if (std::shared_ptr<Node> node = targetNode.lock())
            {
                startScale = node->getScale();
                targetScale = relative ? startScale + scale : scale;

                diff = targetScale - startScale;
            }
        }

        void Scale::updateProgress()
        {
            Animator::updateProgress();

            if (std::shared_ptr<Node> node = targetNode.lock())
            {
                node->setScale(startScale + (diff * progress));
            }
        }
    } // namespace scene
} // namespace ouzel
