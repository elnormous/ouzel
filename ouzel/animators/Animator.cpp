// Copyright (C) 2017 Elviss Strazdins
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
            updateCallback.callback = std::bind(&Animator::update, this, std::placeholders::_1);
        }

        Animator::~Animator()
        {
            if (parent) parent->removeAnimator(this);
        }

        void Animator::update(float delta)
        {
            if (running)
            {
                if (length == 0.0f) // never-ending action
                {
                    currentTime += delta;
                    progress = 0.0f;
                }
                else if (currentTime + delta >= length)
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
            else
            {
                sharedEngine->unscheduleUpdate(&updateCallback);
            }
        }

        void Animator::start()
        {
            sharedEngine->scheduleUpdate(&updateCallback);
            play();
        }

        void Animator::play()
        {
            setProgress(0.0f);
            done = false;
            running = true;

            targetNode = node;

            if (!targetNode && parent)
            {
                targetNode = parent->targetNode;
            }
        }

        void Animator::resume()
        {
            running = true;
        }

        void Animator::stop(bool resetAnimation)
        {
            running = false;

            if (resetAnimation)
            {
                reset();
            }
        }

        void Animator::reset()
        {
            done = false;
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

        void Animator::addAnimator(Animator* animator)
        {
            if (animator) animator->parent = this;
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
            if (animator && animator->parent == this) animator->parent = nullptr;
        }
    } // namespace scene
} // namespace ouzel
