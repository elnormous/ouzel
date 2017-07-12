// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECTSOUND

#include "AudioDS.h"
#include "audio/SoundResource.h"
#include "core/Engine.h"
#include "core/windows/WindowWin.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        AudioDS::AudioDS():
            Audio(Driver::DIRECTSOUND)
        {
        }

        AudioDS::~AudioDS()
        {
            running = false;

#if OUZEL_MULTITHREADED
            if (audioThread.joinable()) audioThread.join();
#endif

            if (buffer) buffer->Release();
            if (primaryBuffer) primaryBuffer->Release();
            if (directSound) directSound->Release();
        }

        bool AudioDS::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            HRESULT hr = DirectSoundCreate8(nullptr, &directSound, nullptr);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create DirectSound 8 instance, error: " << hr;
                return false;
            }

            WindowWin* windowWin = static_cast<WindowWin*>(sharedEngine->getWindow());

            hr = directSound->SetCooperativeLevel(windowWin->getNativeWindow(), DSSCL_PRIORITY);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to set cooperative level for DirectSound 8, error: " << hr;
                return false;
            }

            DSBUFFERDESC primaryBufferDesc;
            primaryBufferDesc.dwSize = sizeof(primaryBufferDesc);
            primaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
            primaryBufferDesc.dwBufferBytes = 0;
            primaryBufferDesc.dwReserved = 0;
            primaryBufferDesc.lpwfxFormat = nullptr;
            primaryBufferDesc.guid3DAlgorithm = GUID_NULL;

            hr = directSound->CreateSoundBuffer(&primaryBufferDesc, &primaryBuffer, nullptr);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create DirectSound buffer, error: " << hr;
                return false;
            }

            WAVEFORMATEX waveFormat;
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = channels;
            waveFormat.nSamplesPerSec = samplesPerSecond;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            hr = primaryBuffer->SetFormat(&waveFormat);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to set DirectSound buffer format, error: " << hr;
                return false;
            }

            IDirectSoundBuffer* tempBuffer = nullptr;

            DSBUFFERDESC bufferDesc;
            bufferDesc.dwSize = sizeof(bufferDesc);
            bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
            bufferDesc.dwBufferBytes = 2 * bufferSize;
            bufferDesc.dwReserved = 0;
            bufferDesc.lpwfxFormat = &waveFormat;
            bufferDesc.guid3DAlgorithm = GUID_NULL;

            hr = directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create DirectSound buffer, error: " << hr;
                return false;
            }

            hr = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, reinterpret_cast<void**>(&buffer));
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create DirectSound buffer, error: " << hr;
                tempBuffer->Release();
                return false;
            }

            tempBuffer->Release();

            uint8_t* bufferPointer;
            DWORD lockedBufferSize;
            hr = buffer->Lock(0, bufferDesc.dwBufferBytes, reinterpret_cast<void**>(&bufferPointer), &lockedBufferSize, nullptr, 0, 0);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to lock DirectSound buffer, error: " << hr;
                return false;
            }

            const std::vector<uint8_t> data = getData(lockedBufferSize);
            std::copy(data.begin(), data.end(), bufferPointer);

            hr = buffer->Unlock(bufferPointer, lockedBufferSize, nullptr, 0);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to unlock DirectSound buffer, error: " << hr;
                return false;
            }

            nextBuffer = 0;

            hr = buffer->Play(0, 0, DSBPLAY_LOOPING);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to play DirectSound buffer, error: " << hr;
                return false;
            }

#if OUZEL_MULTITHREADED
            audioThread = std::thread(&AudioAL::run, this);
#endif

            return true;
        }

        bool AudioDS::update()
        {
            Audio::update();

            DWORD playCursorPosition;
            DWORD writeCursorPosition;
            HRESULT hr = buffer->GetCurrentPosition(&playCursorPosition, &writeCursorPosition);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to get DirectSound buffer cursor position, error: " << hr;
                return false;
            }

            uint32_t currentBuffer = playCursorPosition / bufferSize;

            if (currentBuffer != nextBuffer)
            {
                uint8_t* bufferPointer;
                DWORD lockedBufferSize;
                hr = buffer->Lock(nextBuffer * bufferSize, bufferSize, reinterpret_cast<void**>(&bufferPointer), &lockedBufferSize, nullptr, 0, 0);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to lock DirectSound buffer, error: " << hr;
                    return false;
                }

                const std::vector<uint8_t> data = getData(lockedBufferSize);
                std::copy(data.begin(), data.end(), bufferPointer);

                hr = buffer->Unlock(bufferPointer, lockedBufferSize, nullptr, 0);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to unlock DirectSound buffer, error: " << hr;
                    return false;
                }

                nextBuffer = currentBuffer;
            }

            std::lock_guard<std::mutex> lock(uploadMutex);
            dirty = 0;

            return true;
        }

        void AudioAL::run()
        {
            while (running)
            {
                update();
            }
        }
    } // namespace audio
} // namespace ouzel

#endif
