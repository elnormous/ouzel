// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

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
                if (soundData)
                {
                    if (sourceVoice) sourceVoice->DestroyVoice();

                    WAVEFORMATEX waveFormat;
                    waveFormat.wFormatTag = soundData->getFormatTag();
                    waveFormat.nChannels = soundData->getChannels();
                    waveFormat.nSamplesPerSec = soundData->getSamplesPerSecond();
                    waveFormat.wBitsPerSample = 16;
                    waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
                    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
                    waveFormat.cbSize = 0;

                    AudioXA2* audioXA2 = static_cast<AudioXA2*>(sharedEngine->getAudio());
                    sourceVoice = audioXA2->createSourceVoice(waveFormat);

                    if (!sourceVoice)
                    {
                        return false;
                    }

                    channels = soundData->getChannels();
                }
                else
                {
                    if (sourceVoice)
                    {
                        sourceVoice->DestroyVoice();
                        sourceVoice = nullptr;
                    }

                    channels = 0;
                }
            }

            if (dirty & DIRTY_PLAY_STATE)
            {
                if (sourceVoice)
                {
                    if (shouldPlay)
                    {
                        if (reset)
                        {
                            HRESULT hr = sourceVoice->FlushSourceBuffers();
                            if (FAILED(hr))
                            {
                                Log(Log::Level::ERR) << "Failed to flush sound buffer, error: " << hr;
                                return false;
                            }

                        }

                        const std::vector<uint8_t> data = soundData->getData();

                        XAUDIO2_BUFFER bufferData;
                        bufferData.Flags = XAUDIO2_END_OF_STREAM;
                        bufferData.AudioBytes = static_cast<UINT32>(data.size());
                        bufferData.pAudioData = data.data();
                        bufferData.PlayBegin = 0;
                        bufferData.PlayLength = 0;
                        bufferData.LoopBegin = 0;
                        bufferData.LoopLength = 0;
                        bufferData.LoopCount = repeat ? XAUDIO2_LOOP_INFINITE : 0;
                        bufferData.pContext = nullptr;

                        HRESULT hr = sourceVoice->SubmitSourceBuffer(&bufferData);
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to upload sound data, error: " << hr;
                            return false;
                        }

                        hr = sourceVoice->Start();
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to start consuming sound data, error: " << hr;
                            return false;
                        }
                    }
                    else
                    {
                        HRESULT hr = sourceVoice->Stop();
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to stop sound buffer, error: " << hr;
                            return false;
                        }

                        if (reset)
                        {
                            hr = sourceVoice->FlushSourceBuffers();
                            if (FAILED(hr))
                            {
                                Log(Log::Level::ERR) << "Failed to flush sound buffer, error: " << hr;
                                return false;
                            }
                        
                        }
                    }
                }

                reset = false;
            }

            dirty = 0;

            if (sourceVoice && channels == 1)
            {
                // TODO: update position
                // sourceVoice->SetOutputMatrix(nullptr, channels, outputMatrix);
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel

#endif
