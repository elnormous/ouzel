// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Sound.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound(Audio& initAudio, uintptr_t initSourceDataId):
            audio(initAudio),
            sourceDataId(initSourceDataId)
        {
        }

        Sound::~Sound()
        {
            if (sourceDataId) audio.deleteObject(sourceDataId);
        }
    } // namespace audio
} // namespace ouzel
