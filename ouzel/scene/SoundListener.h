// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.h"

namespace ouzel
{
    namespace scene
    {
        class SoundListener: public Component
        {
        public:
            static const uint32_t TYPE = Component::SOUND_LISTENER;

            SoundListener();
        };
    } // namespace scene
} // namespace ouzel
