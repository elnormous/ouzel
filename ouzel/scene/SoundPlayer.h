// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.h"
#include "audio/Sound.h"
#include "audio/SoundData.h"

namespace ouzel
{
    namespace scene
    {
        class SoundPlayer: public Component
        {
        public:
            static const uint32_t TYPE = Component::SOUND_PLAYER;

            SoundPlayer();
            SoundPlayer(const std::shared_ptr<audio::SoundData>& aSoundData);
            SoundPlayer(const std::string& filename);

            bool init(const std::shared_ptr<audio::SoundData>& newSoundData);
            bool init(const std::string& filename);

        protected:
            std::shared_ptr<audio::Sound> sound;
            std::shared_ptr<audio::SoundData> soundData;
        };
    } // namespace scene
} // namespace ouzel
