// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Audio.h"
#include "SoundData.h"
#include "Sound.h"

namespace ouzel
{
    namespace audio
    {
        Audio::Audio(Driver aDriver):
            driver(aDriver)
        {
        }

        Audio::~Audio()
        {
        }

        void Audio::free()
        {
            ready = false;
        }

        bool Audio::init()
        {
            ready = true;

            return true;
        }
    } // namespace audio
} // namespace ouzel
