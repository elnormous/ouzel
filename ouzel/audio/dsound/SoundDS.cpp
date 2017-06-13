// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDS.h"
#include "AudioDS.h"
#include "audio/SoundData.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        SoundDS::SoundDS()
        {
        }

        SoundDS::~SoundDS()
        {
            if (buffer3D) buffer3D->Release();
            if (buffer) buffer->Release();
        }

        bool SoundDS::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty & DIRTY_SOUND_DATA)
            {
                if (soundData)
                {
                    if (buffer3D)
                    {
                        buffer3D->Release();
                        buffer3D = nullptr;
                    }

                    if (buffer)
                    {
                        buffer->Release();
                        buffer = nullptr;
                    }

                    IDirectSoundBuffer* tempBuffer = nullptr;

                    WAVEFORMATEX waveFormat;
                    waveFormat.wFormatTag = soundData->getFormatTag();
                    waveFormat.nChannels = soundData->getChannels();
                    waveFormat.nSamplesPerSec = soundData->getSamplesPerSecond();
                    waveFormat.wBitsPerSample = 16;
                    waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
                    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
                    waveFormat.cbSize = 0;

                    const std::vector<uint8_t> data = soundData->getData();

                    DSBUFFERDESC bufferDesc;
                    bufferDesc.dwSize = sizeof(bufferDesc);
                    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;

                    if (soundData->getChannels() < 2)
                    {
                        bufferDesc.dwFlags |= DSBCAPS_CTRL3D;
                    }

                    bufferDesc.dwBufferBytes = static_cast<DWORD>(data.size());
                    bufferDesc.dwReserved = 0;
                    bufferDesc.lpwfxFormat = &waveFormat;
                    bufferDesc.guid3DAlgorithm = GUID_NULL;

                    AudioDS* audioDS = static_cast<AudioDS*>(sharedEngine->getAudio());
                    HRESULT hr;
                    if (FAILED(hr = audioDS->getDirectSound()->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr)))
                    {
                        Log(Log::Level::ERR) << "Failed to create DirectSound buffer";
                        return false;
                    }

                    if (FAILED(tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, reinterpret_cast<void**>(&buffer))))
                    {
                        Log(Log::Level::ERR) << "Failed to create DirectSound buffer";
                        tempBuffer->Release();
                        return false;
                    }

                    tempBuffer->Release();

                    uint8_t* bufferPointer;
                    DWORD bufferSize;

                    if (FAILED(buffer->Lock(0, bufferDesc.dwBufferBytes, reinterpret_cast<void**>(&bufferPointer), &bufferSize, nullptr, 0, 0)))
                    {
                        Log(Log::Level::ERR) << "Failed to lock DirectSound buffer";
                        return false;
                    }

                    std::copy(data.begin(), data.end(), bufferPointer);

                    if (FAILED(buffer->Unlock(bufferPointer, bufferSize, nullptr, 0)))
                    {
                        Log(Log::Level::ERR) << "Failed to unlock DirectSound buffer";
                        return false;
                    }

                    if (bufferDesc.dwFlags & DSBCAPS_CTRL3D)
                    {
                        if (FAILED(buffer->QueryInterface(IID_IDirectSound3DBuffer8, reinterpret_cast<void**>(&buffer3D))))
                        {
                            Log(Log::Level::ERR) << "Failed to get DirectSound 3D buffer";
                            return false;
                        }
                    }
                }
            }

            if (dirty & DIRTY_POSITION)
            {
                if (buffer3D)
                {
                    if (FAILED(buffer3D->SetPosition(position.v[0], position.v[1], position.v[2], DS3D_IMMEDIATE)))
                    {
                        Log(Log::Level::ERR) << "Failed to set DirectSound buffer position";
                        return false;
                    }
                }
            }

            if (dirty & DIRTY_PLAY_STATE)
            {
                if (buffer)
                {
                    if (shouldPlay)
                    {
                        if (reset)
                        {
                            if (FAILED(buffer->SetCurrentPosition(0)))
                            {
                                Log(Log::Level::ERR) << "Failed to set DirectSound buffer current position";
                                return false;
                            }
                        }

                        if (FAILED(buffer->Play(0, 0, 0)))
                        {
                            Log(Log::Level::ERR) << "Failed to play DirectSound buffer";
                            return false;
                        }
                    }
                    else
                    {
                        if (FAILED(buffer->Stop()))
                        {
                            Log(Log::Level::ERR) << "Failed to stop DirectSound buffer";
                            return false;
                        }

                        if (FAILED(buffer->SetCurrentPosition(0)))
                        {
                            Log(Log::Level::ERR) << "Failed to set DirectSound buffer current position";
                            return false;
                        }
                    }
                }
            }

            dirty = 0;

            return true;
        }
    } // namespace audio
} // namespace ouzel
