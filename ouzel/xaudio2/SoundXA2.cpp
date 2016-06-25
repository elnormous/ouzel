// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundXA2.h"
#include "AudioXA2.h"
#include "SoundDataXA2.h"
#include "core/Engine.h"
#include "utils/Utils.h"

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

        void SoundXA2::free()
        {
            Sound::free();

            if (sourceVoice)
            {
                sourceVoice->DestroyVoice();
                sourceVoice = nullptr;
            }
        }

        bool SoundXA2::init(const SoundDataPtr& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            free();

            std::shared_ptr<AudioXA2> audioXA2 = std::static_pointer_cast<AudioXA2>(sharedEngine->getAudio());
            std::shared_ptr<SoundDataXA2> soundDataXA2 = std::static_pointer_cast<SoundDataXA2>(soundData);

            if (FAILED(audioXA2->getXAudio()->CreateSourceVoice(&sourceVoice, &soundDataXA2->getWaveFormat())))
            {
                log("Failed to create source voice");
                return false;
            }

            ready = true;

            return true;
        }

        bool SoundXA2::play(bool repeatSound)
        {
            if (!Sound::play(repeatSound))
            {
                return false;
            }
            
            XAUDIO2_BUFFER bufferData;
            bufferData.Flags = XAUDIO2_END_OF_STREAM;
            bufferData.AudioBytes = static_cast<UINT32>(soundData->getData().size());
            bufferData.pAudioData = soundData->getData().data();
            bufferData.PlayBegin = 0;
            bufferData.PlayLength = 0;
            bufferData.LoopBegin = 0;
            bufferData.LoopLength = 0;
            bufferData.LoopCount = 0; // XAUDIO2_LOOP_INFINITE
            bufferData.pContext = nullptr;

            if (FAILED(sourceVoice->SubmitSourceBuffer(&bufferData)))
            {
                log("Failed to upload sound data");
                return false;
            }

            if (FAILED(sourceVoice->Start()))
            {
                log("Failed to start consuming sound data");
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
                log("Failed to stop sound buffer");
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
                log("Failed to stop sound buffer");
                return false;
            }

            if (FAILED(sourceVoice->FlushSourceBuffers()))
            {
                log("Failed to flush sound buffer");
                return false;
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
