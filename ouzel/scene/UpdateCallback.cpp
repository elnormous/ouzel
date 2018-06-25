// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "UpdateCallback.hpp"
#include "SceneManager.hpp"

namespace ouzel
{
    namespace scene
    {
        UpdateCallback::~UpdateCallback()
        {
            if (sceneManager) sceneManager->unscheduleUpdate(this);
        }

        void UpdateCallback::remove()
        {
            if (sceneManager)
            {
                sceneManager->unscheduleUpdate(this);
                sceneManager = nullptr;
            }
        }
    }
}
