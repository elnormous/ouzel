// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Sound.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound(Audio& initAudio, uintptr_t initSourceId):
            audio(initAudio),
            sourceId(initSourceId)
        {
        }

        Sound::~Sound()
        {
            if (sourceId) audio.deleteObject(sourceId);
        }
    } // namespace audio
} // namespace ouzel
