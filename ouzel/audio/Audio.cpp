// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Audio.h"
#include "SoundData.h"
#include "Sound.h"
#include "SoundResource.h"

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

        bool Audio::init()
        {
            ready = true;

            return true;
        }

        bool Audio::process()
        {
            return true;
        }

        void Audio::setListenerPosition(const Vector3& newPosition)
        {
            listenerPosition = newPosition;
        }

        void Audio::setListenerOrientation(const Vector3& newOrientation)
        {
            listenerOrientation = newOrientation;
        }
    } // namespace audio
} // namespace ouzel
