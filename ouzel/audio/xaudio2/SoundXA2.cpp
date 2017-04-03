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

        bool SoundXA2::update()
        {
            if (dirty & DIRTY_SOUND_DATA)
            {
                if (sourceVoice) sourceVoice->DestroyVoice();

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
            }

            if (dirty & DIRTY_PLAY_STATE)
            {
                if (shouldPlay)
                {
                    if (reset)
                    {
                        if (FAILED(sourceVoice->FlushSourceBuffers()))
                        {
                            Log(Log::Level::ERR) << "Failed to flush sound buffer";
                            return false;
                        }

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
                    bufferData.LoopCount = repeat ? XAUDIO2_LOOP_INFINITE : 0;
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
                }
                else
                {
                    if (FAILED(sourceVoice->Stop()))
                    {
                        Log(Log::Level::ERR) << "Failed to stop sound buffer";
                        return false;
                    }

                    if (reset)
                    {
                        if (FAILED(sourceVoice->FlushSourceBuffers()))
                        {
                            Log(Log::Level::ERR) << "Failed to flush sound buffer";
                            return false;
                        }
                        
                    }
                }

                reset = false;
            }

            dirty = 0;

            return true;
        }
    } // namespace audio
} // namespace ouzel
