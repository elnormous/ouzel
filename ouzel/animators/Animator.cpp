// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
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
            for (Animator* animator : animators)
            {
                animator->parent = nullptr;
            }

            if (parent) parent->removeAnimator(this);
        }

        void Animator::setParent(Animator* newParent)
        {
            if (parent)
            {
                parent->removeAnimator(this);
            }

            parent = newParent;

            if (parent)
            {
                parent->addAnimator(this);
            }
        }

        void Animator::removeFromParent()
        {
            if (parent)
            {
                parent->removeAnimator(this);
                parent = nullptr;
            }
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

            for (Animator* animator : animators)
            {
                animator->play();
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

            for (Animator* animator : animators)
            {
                animator->reset();
            }
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
            animators.push_back(animator);
            animator->parent = this;
        }

        bool Animator::removeAnimator(Animator* animator)
        {
            auto i = std::find(animators.begin(), animators.end(), animator);

            if (i != animators.end())
            {
                animator->parent = nullptr;
                animators.erase(i);
                return true;
            }

            return true;
        }
    } // namespace scene
} // namespace ouzel
