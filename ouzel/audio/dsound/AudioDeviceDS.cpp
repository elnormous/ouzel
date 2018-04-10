// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

#include "AudioDeviceDS.hpp"
#include "core/Engine.hpp"
#include "core/windows/WindowResourceWin.hpp"
#include "utils/Log.hpp"

BOOL CALLBACK enumCallback(LPGUID, LPCWSTR description, LPCWSTR, LPVOID)
{
    char temp[256];
    WideCharToMultiByte(CP_UTF8, 0, description, -1, temp, sizeof(temp), nullptr, nullptr);

    ouzel::Log(ouzel::Log::Level::INFO) << "Using " << temp << " for audio";
    return FALSE;
}

namespace ouzel
{
    namespace audio
    {
        AudioDeviceDS::AudioDeviceDS():
            AudioDevice(Audio::Driver::DIRECTSOUND), running(false)
        {
            std::fill(std::begin(notifyEvents), std::end(notifyEvents), INVALID_HANDLE_VALUE);
        }

        AudioDeviceDS::~AudioDeviceDS()
        {
            running = false;
            for (HANDLE notifyEvent : notifyEvents)
            {
                SetEvent(notifyEvent);
            }
            if (audioThread.isJoinable()) audioThread.join();

            for (HANDLE notifyEvent : notifyEvents)
            {
                if (notifyEvent != INVALID_HANDLE_VALUE) CloseHandle(notifyEvent);
            }

            if (notify) notify->Release();
            if (buffer) buffer->Release();
            if (primaryBuffer) primaryBuffer->Release();
            if (directSound) directSound->Release();
        }

        bool AudioDeviceDS::init(bool debugAudio)
        {
            if (!AudioDevice::init(debugAudio))
            {
                return false;
            }

            HRESULT hr = DirectSoundEnumerateW(enumCallback, this);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to enumerate DirectSound 8 devices, error: " << hr;
                return false;
            }

            hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &directSound, nullptr);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to create DirectSound 8 instance, error: " << hr;
                return false;
            }

            WindowResourceWin* windowWin = static_cast<WindowResourceWin*>(engine->getWindow()->getResource());

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
            waveFormat.nSamplesPerSec = sampleRate;
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

            sampleFormat = Audio::SampleFormat::SINT16;

            IDirectSoundBuffer* tempBuffer = nullptr;

            DSBUFFERDESC bufferDesc;
            bufferDesc.dwSize = sizeof(bufferDesc);
            bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY;
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

            hr = tempBuffer->QueryInterface(IID_IDirectSoundNotify, reinterpret_cast<void**>(&notify));
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to get DirectSound notify interface, error: " << hr;
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

            getData(lockedBufferSize / (channels * sizeof(int16_t)), data);
            std::copy(data.begin(), data.end(), bufferPointer);

            hr = buffer->Unlock(bufferPointer, lockedBufferSize, nullptr, 0);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to unlock DirectSound buffer, error: " << hr;
                return false;
            }

            nextBuffer = 0;

            notifyEvents[0] = CreateEvent(nullptr, true, false, nullptr);
            notifyEvents[1] = CreateEvent(nullptr, true, false, nullptr);

            DSBPOSITIONNOTIFY positionNotifyEvents[2];
            positionNotifyEvents[0].dwOffset = bufferSize - 1;
            positionNotifyEvents[0].hEventNotify = notifyEvents[0];

            positionNotifyEvents[1].dwOffset = 2 * bufferSize - 1;
            positionNotifyEvents[1].hEventNotify = notifyEvents[1];

            hr = notify->SetNotificationPositions(2, positionNotifyEvents);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to set DirectSound notification positions, error: " << hr;
                return false;
            }

            hr = buffer->Play(0, 0, DSBPLAY_LOOPING);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to play DirectSound buffer, error: " << hr;
                return false;
            }

            running = true;
            audioThread = Thread(std::bind(&AudioDeviceDS::run, this), "Audio");

            return true;
        }

        void AudioDeviceDS::run()
        {
            for (;;)
            {
                if (WaitForSingleObject(notifyEvents[nextBuffer], INFINITE) == WAIT_OBJECT_0)
                {
                    if (!running) break;

                    ResetEvent(notifyEvents[nextBuffer]);

                    if (!process())
                    {
                        break;
                    }

                    uint8_t* bufferPointer;
                    DWORD lockedBufferSize;
                    HRESULT hr = buffer->Lock(nextBuffer * bufferSize, bufferSize, reinterpret_cast<void**>(&bufferPointer), &lockedBufferSize, nullptr, 0, 0);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to lock DirectSound buffer, error: " << hr;
                        break;
                    }

                    if (!getData(lockedBufferSize / (channels * sizeof(int16_t)), data))
                    {
                        break;
                    }

                    std::copy(data.begin(), data.end(), bufferPointer);

                    hr = buffer->Unlock(bufferPointer, lockedBufferSize, nullptr, 0);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to unlock DirectSound buffer, error: " << hr;
                        break;
                    }

                    nextBuffer = (nextBuffer + 1) % 2;
                }
            }
        }
    } // namespace audio
} // namespace ouzel

#endif
