// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

#include "AudioDeviceDS.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

BOOL CALLBACK enumCallback(LPGUID, LPCWSTR description, LPCWSTR, LPVOID)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, description, -1, nullptr, 0, nullptr, nullptr);
    if (bufferSize != 0)
    {
        std::vector<char> buffer(bufferSize);
        if (WideCharToMultiByte(CP_UTF8, 0, description, -1, buffer.data(), bufferSize, nullptr, nullptr) != 0)
            ouzel::engine->log(ouzel::Log::Level::INFO) << "Using " << buffer.data() << " for audio";
    }
    return FALSE;
}

namespace ouzel
{
    namespace audio
    {
        AudioDeviceDS::AudioDeviceDS(Window* window):
            AudioDevice(Audio::Driver::DIRECTSOUND), running(true)
        {
            std::fill(std::begin(notifyEvents), std::end(notifyEvents), INVALID_HANDLE_VALUE);

            HRESULT hr = DirectSoundEnumerateW(enumCallback, this);
            if (FAILED(hr))
                throw SystemError("Failed to enumerate DirectSound 8 devices, error: " + std::to_string(hr));

            hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &directSound, nullptr);
            if (FAILED(hr))
                throw SystemError("Failed to create DirectSound 8 instance, error: " + std::to_string(hr));

            NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(window->getNativeWindow());

            hr = directSound->SetCooperativeLevel(windowWin->getNativeWindow(), DSSCL_PRIORITY);
            if (FAILED(hr))
                throw SystemError("Failed to set cooperative level for DirectSound 8, error: " + std::to_string(hr));

            DSBUFFERDESC primaryBufferDesc;
            primaryBufferDesc.dwSize = sizeof(primaryBufferDesc);
            primaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
            primaryBufferDesc.dwBufferBytes = 0;
            primaryBufferDesc.dwReserved = 0;
            primaryBufferDesc.lpwfxFormat = nullptr;
            primaryBufferDesc.guid3DAlgorithm = GUID_NULL;

            hr = directSound->CreateSoundBuffer(&primaryBufferDesc, &primaryBuffer, nullptr);
            if (FAILED(hr))
                throw SystemError("Failed to create DirectSound buffer, error: " + std::to_string(hr));

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
                throw SystemError("Failed to set DirectSound buffer format, error: " + std::to_string(hr));

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
                throw SystemError("Failed to create DirectSound buffer, error: " + std::to_string(hr));

            hr = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, reinterpret_cast<void**>(&buffer));
            if (FAILED(hr))
            {
                tempBuffer->Release();
                throw SystemError("Failed to create DirectSound buffer, error: " + std::to_string(hr));
            }

            hr = tempBuffer->QueryInterface(IID_IDirectSoundNotify, reinterpret_cast<void**>(&notify));
            if (FAILED(hr))
            {
                tempBuffer->Release();
                throw SystemError("Failed to get DirectSound notify interface, error: " + std::to_string(hr));
            }

            tempBuffer->Release();

            uint8_t* bufferPointer;
            DWORD lockedBufferSize;
            hr = buffer->Lock(0, bufferDesc.dwBufferBytes, reinterpret_cast<void**>(&bufferPointer), &lockedBufferSize, nullptr, 0, 0);
            if (FAILED(hr))
                throw SystemError("Failed to lock DirectSound buffer, error: " + std::to_string(hr));

            getData(lockedBufferSize / (channels * sizeof(int16_t)), data);
            std::copy(data.begin(), data.end(), bufferPointer);

            hr = buffer->Unlock(bufferPointer, lockedBufferSize, nullptr, 0);
            if (FAILED(hr))
                throw SystemError("Failed to unlock DirectSound buffer, error: " + std::to_string(hr));

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
                throw SystemError("Failed to set DirectSound notification positions, error: " + std::to_string(hr));

            hr = buffer->Play(0, 0, DSBPLAY_LOOPING);
            if (FAILED(hr))
                throw SystemError("Failed to play DirectSound buffer, error: " + std::to_string(hr));

            audioThread = std::thread(&AudioDeviceDS::run, this);
        }

        AudioDeviceDS::~AudioDeviceDS()
        {
            running = false;
            for (HANDLE notifyEvent : notifyEvents)
                SetEvent(notifyEvent);

            if (audioThread.joinable()) audioThread.join();

            for (HANDLE notifyEvent : notifyEvents)
                if (notifyEvent != INVALID_HANDLE_VALUE) CloseHandle(notifyEvent);

            if (notify) notify->Release();
            if (buffer) buffer->Release();
            if (primaryBuffer) primaryBuffer->Release();
            if (directSound) directSound->Release();
        }

        void AudioDeviceDS::run()
        {
            setCurrentThreadName("Audio");

            while (running)
            {
                try
                {
                    if (WaitForSingleObject(notifyEvents[nextBuffer], INFINITE) == WAIT_OBJECT_0)
                    {
                        if (!running) break;

                        ResetEvent(notifyEvents[nextBuffer]);

                        process();

                        uint8_t* bufferPointer;
                        DWORD lockedBufferSize;
                        HRESULT hr = buffer->Lock(nextBuffer * bufferSize, bufferSize, reinterpret_cast<void**>(&bufferPointer), &lockedBufferSize, nullptr, 0, 0);
                        if (FAILED(hr))
                            throw SystemError("Failed to lock DirectSound buffer, error: " + std::to_string(hr));

                        getData(lockedBufferSize / (channels * sizeof(int16_t)), data);

                        std::copy(data.begin(), data.end(), bufferPointer);

                        hr = buffer->Unlock(bufferPointer, lockedBufferSize, nullptr, 0);
                        if (FAILED(hr))
                            throw SystemError("Failed to unlock DirectSound buffer, error: " + std::to_string(hr));

                        nextBuffer = (nextBuffer + 1) % 2;
                    }
                }
                catch (const std::exception& e)
                {
                    engine->log(Log::Level::ERR) << e.what();
                }
            }
        }
    } // namespace audio
} // namespace ouzel

#endif
