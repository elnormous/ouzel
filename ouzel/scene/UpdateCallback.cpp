// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
