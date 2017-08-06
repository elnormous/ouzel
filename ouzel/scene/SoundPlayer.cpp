// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundPlayer.hpp"
#include "core/Engine.hpp"
#include "core/Cache.hpp"

namespace ouzel
{
    namespace scene
    {
        SoundPlayer::SoundPlayer():
            Component(TYPE)
        {
        }

        SoundPlayer::SoundPlayer(const std::shared_ptr<audio::SoundData>& aSoundData):
            SoundPlayer()
        {
            init(aSoundData);
        }

        SoundPlayer::SoundPlayer(const std::string& filename):
            SoundPlayer()
        {
            init(filename);
        }

        bool SoundPlayer::init(const std::shared_ptr<audio::SoundData>& newSoundData)
        {
            soundData = newSoundData;

            sound = std::make_shared<audio::Sound>();
            if (!sound->init(soundData))
            {
                return false;
            }

            return true;
        }

        bool SoundPlayer::init(const std::string& filename)
        {
            soundData = sharedEngine->getCache()->getSoundData(filename);

            sound = std::make_shared<audio::Sound>();
            if (!sound->init(soundData))
            {
                return false;
            }

            return true;
        }
    } // namespace scene
} // namespace ouzel
