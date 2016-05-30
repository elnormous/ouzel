// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Animator.h"
#include "utils/Utils.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace scene
    {
        Animator::Animator(float pLength):
            length(pLength)
        {
            updateCallback = std::make_shared<UpdateCallback>();
            updateCallback->callback = std::bind(&Animator::update, this, std::placeholders::_1);
        }

        Animator::~Animator()
        {
            sharedEngine->unscheduleUpdate(updateCallback);
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
                    sharedEngine->unscheduleUpdate(updateCallback);
                }
                else
                {
                    currentTime += delta;
                    progress = currentTime / length;
                }

                updateProgress();
            }
        }

        void Animator::start(const NodePtr& targetNode)
        {
            if (!running)
            {
                running = true;
                node = targetNode;

                sharedEngine->scheduleUpdate(updateCallback);
            }
        }

        void Animator::resume()
        {
            if (!running)
            {
                running = true;
                sharedEngine->scheduleUpdate(updateCallback);
            }
        }

        void Animator::stop(bool resetAnimation)
        {
            if (running)
            {
                running = false;
                sharedEngine->unscheduleUpdate(updateCallback);
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

    } // namespace scene
} // namespace ouzel
