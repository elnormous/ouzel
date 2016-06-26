// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSL.h"
#include "AudioSL.h"
#include "SoundDataSL.h"
#include "core/Engine.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        SoundSL::SoundSL()
        {
        }

        SoundSL::~SoundSL()
        {
        }

        void SoundSL::free()
        {
            Sound::free();
        }

        bool SoundSL::init(const SoundDataPtr& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            free();

            ready = true;

            return true;
        }

        bool SoundSL::play(bool repeatSound)
        {
            if (!Sound::play(repeatSound))
            {
                return false;
            }

            return true;
        }

        bool SoundSL::stop(bool resetSound)
        {
            if (!Sound::stop(resetSound))
            {
                return false;
            }

            return true;
        }

        bool SoundSL::reset()
        {
            if (!Sound::reset())
            {
                return false;
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
