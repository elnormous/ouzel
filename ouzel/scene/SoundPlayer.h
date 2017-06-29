// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.h"

namespace ouzel
{
    namespace scene
    {
        class SoundPlayer: public Component
        {
        public:
            static const uint32_t componentType = Component::SOUND_PLAYER;

            SoundPlayer();
        };
    } // namespace scene
} // namespace ouzel
