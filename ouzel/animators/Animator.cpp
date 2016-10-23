// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Animator.h"
#include "utils/Utils.h"
#include "core/Engine.h"
#include "scene/Node.h"

namespace ouzel
{
    namespace scene
    {
        Animator::Animator(float aLength):
            length(aLength)
        {
        }

        Animator::~Animator()
        {
            if (parent) parent->removeAnimator(this);
            if (parentNode) parentNode->removeAnimator(this);
        }

        void Animator::update(float delta)
        {
            if (running)
            {
                if (currentTime + delta >= length)
                {
                    done = true;
                    running = false;
                    progress = 1.0f;
                    currentTime = length;
                    if (finishHandler) finishHandler();
                }
                else
                {
                    currentTime += delta;
                    progress = currentTime / length;
                }

                updateProgress();
            }
        }

        void Animator::start(Node* newTargetNode)
        {
            targetNode = newTargetNode;
            running = true;
        }

        void Animator::resume()
        {
            if (!running)
            {
                running = true;
            }
        }

        void Animator::stop(bool resetAnimation)
        {
            if (running)
            {
                running = false;
            }

            if (resetAnimation)
            {
                reset();
            }
        }

        void Animator::reset()
        {
            done = false;
            currentTime = 0.0f;
            setProgress(0.0f);
        }

        void Animator::setProgress(float newProgress)
        {
            progress = newProgress;
            currentTime = progress * length;

            updateProgress();
        }

        void Animator::updateProgress()
        {
        }

        void Animator::removeFromParent()
        {
            if (parent)
            {
                parent->removeAnimator(this);
                parent = nullptr;
            }
        }

        void Animator::removeAnimator(Animator* animator)
        {
            if (animator->parent == this) animator->parent = nullptr;
        }
    } // namespace scene
} // namespace ouzel
