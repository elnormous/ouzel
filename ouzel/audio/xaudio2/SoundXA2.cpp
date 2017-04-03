// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundXA2.h"
#include "AudioXA2.h"
#include "audio/SoundData.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        SoundXA2::SoundXA2()
        {
        }

        SoundXA2::~SoundXA2()
        {
            if (sourceVoice) sourceVoice->DestroyVoice();
        }

        bool SoundXA2::init(const std::shared_ptr<SoundData>& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            WAVEFORMATEX waveFormat;
            waveFormat.wFormatTag = soundData->getFormatTag();
            waveFormat.nChannels = soundData->getChannels();
            waveFormat.nSamplesPerSec = soundData->getSamplesPerSecond();
            waveFormat.nBlockAlign = soundData->getChannels() * 2; // 16 bits
            waveFormat.nAvgBytesPerSec = soundData->getSamplesPerSecond() * waveFormat.nBlockAlign; // 16 bits
            waveFormat.wBitsPerSample = 16;
            waveFormat.cbSize = 0;

            AudioXA2* audioXA2 = static_cast<AudioXA2*>(sharedEngine->getAudio());
            sourceVoice = audioXA2->createSourceVoice(waveFormat);

            if (!sourceVoice)
            {
                return false;
            }

            return true;
        }

        void SoundXA2::setPosition(const Vector3& newPosition)
        {
            Sound::setPosition(newPosition);
        }

        void SoundXA2::setGain(float newGain)
        {
            Sound::setGain(newGain);
            gain = newGain;
        }

        bool SoundXA2::play(bool repeatSound)
        {
            if (!Sound::play(repeatSound))
            {
                return false;
            }

            soundData->getData(buffer);

            XAUDIO2_BUFFER bufferData;
            bufferData.Flags = XAUDIO2_END_OF_STREAM;
            bufferData.AudioBytes = static_cast<UINT32>(buffer.size());
            bufferData.pAudioData = buffer.data();
            bufferData.PlayBegin = 0;
            bufferData.PlayLength = 0;
            bufferData.LoopBegin = 0;
            bufferData.LoopLength = 0;
            bufferData.LoopCount = repeatSound ? XAUDIO2_LOOP_INFINITE : 0;
            bufferData.pContext = nullptr;

            if (FAILED(sourceVoice->SubmitSourceBuffer(&bufferData)))
            {
                Log(Log::Level::ERR) << "Failed to upload sound data";
                return false;
            }

            if (FAILED(sourceVoice->Start()))
            {
                Log(Log::Level::ERR) << "Failed to start consuming sound data";
                return false;
            }

            return true;
        }

        bool SoundXA2::stop(bool resetSound)
        {
            if (!Sound::stop(resetSound))
            {
                return false;
            }

            if (FAILED(sourceVoice->Stop()))
            {
                Log(Log::Level::ERR) << "Failed to stop sound buffer";
                return false;
            }

            return true;
        }

        bool SoundXA2::reset()
        {
            if (!Sound::reset())
            {
                return false;
            }

            if (FAILED(sourceVoice->Stop()))
            {
                Log(Log::Level::ERR) << "Failed to stop sound buffer";
                return false;
            }

            if (FAILED(sourceVoice->FlushSourceBuffers()))
            {
                Log(Log::Level::ERR) << "Failed to flush sound buffer";
                return false;
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
