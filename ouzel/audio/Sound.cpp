// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Sound.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound(Audio& initAudio):
            audio(initAudio)
        {
        }

        Sound::~Sound()
        {
        }

        void Sound::getData(Stream* stream, uint32_t frames, uint32_t neededChannels, uint32_t neededSampleRate, std::vector<float>& result)
        {
            uint32_t neededFrames = static_cast<uint32_t>(frames * sampleRate / neededSampleRate);

            readData(stream, neededFrames, tempData);

            resampledData = tempData;

            result = resampledData;
        }
    } // namespace audio
} // namespace ouzel
