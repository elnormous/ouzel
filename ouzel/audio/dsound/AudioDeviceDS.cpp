// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

#include <system_error>
#include "AudioDeviceDS.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/windows/NativeWindowWin.hpp"
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
        class DirectSoundErrorCategory: public std::error_category
        {
        public:
            const char* name() const noexcept override
            {
                return "DirectSound";
            }

            std::string message(int condition) const override
            {
                switch (condition)
                {
                    case DSERR_ACCESSDENIED: return "DSERR_ACCESSDENIED";
                    case DSERR_ALLOCATED: return "DSERR_ALLOCATED";
                    case DSERR_ALREADYINITIALIZED: return "DSERR_ALREADYINITIALIZED";
                    case DSERR_BADFORMAT: return "DSERR_BADFORMAT";
                    case DSERR_BADSENDBUFFERGUID: return "DSERR_BADSENDBUFFERGUID";
                    case DSERR_BUFFERLOST: return "DSERR_BUFFERLOST";
                    case DSERR_BUFFERTOOSMALL: return "DSERR_BUFFERTOOSMALL";
                    case DSERR_CONTROLUNAVAIL: return "DSERR_CONTROLUNAVAIL";
                    case DSERR_DS8_REQUIRED: return "DSERR_DS8_REQUIRED";
                    case DSERR_FXUNAVAILABLE: return "DSERR_FXUNAVAILABLE";
                    case DSERR_GENERIC: return "DSERR_GENERIC";
                    case DSERR_INVALIDCALL: return "DSERR_INVALIDCALL";
                    case DSERR_INVALIDPARAM: return "DSERR_INVALIDPARAM";
                    case DSERR_NOAGGREGATION: return "DSERR_NOAGGREGATION";
                    case DSERR_NODRIVER: return "DSERR_NODRIVER";
                    case DSERR_NOINTERFACE: return "DSERR_NOINTERFACE";
                    case DSERR_OBJECTNOTFOUND: return "DSERR_OBJECTNOTFOUND";
                    case DSERR_OTHERAPPHASPRIO: return "DSERR_OTHERAPPHASPRIO";
                    case DSERR_OUTOFMEMORY: return "DSERR_OUTOFMEMORY";
                    case DSERR_PRIOLEVELNEEDED: return "DSERR_PRIOLEVELNEEDED";
                    case DSERR_SENDLOOP: return "DSERR_SENDLOOP";
                    case DSERR_UNINITIALIZED: return "DSERR_UNINITIALIZED";
                    case DSERR_UNSUPPORTED: return "DSERR_UNSUPPORTED";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        const DirectSoundErrorCategory directSoundErrorCategory {};

        AudioDeviceDS::AudioDeviceDS(Window* window):
            AudioDevice(Driver::DIRECTSOUND), running(true)
        {
            HRESULT hr;
            if (FAILED(hr = DirectSoundEnumerateW(enumCallback, this)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to enumerate DirectSound 8 devices");

            if (FAILED(hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &directSound, nullptr)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to create DirectSound 8 instance");

            NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(window->getNativeWindow());

            if (FAILED(hr = directSound->SetCooperativeLevel(windowWin->getNativeWindow(), DSSCL_PRIORITY)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to set cooperative level for DirectSound 8");

            DSBUFFERDESC primaryBufferDesc;
            primaryBufferDesc.dwSize = sizeof(primaryBufferDesc);
            primaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
            primaryBufferDesc.dwBufferBytes = 0;
            primaryBufferDesc.dwReserved = 0;
            primaryBufferDesc.lpwfxFormat = nullptr;
            primaryBufferDesc.guid3DAlgorithm = GUID_NULL;

            if (FAILED(hr = directSound->CreateSoundBuffer(&primaryBufferDesc, &primaryBuffer, nullptr)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to create DirectSound buffer");

            WAVEFORMATEX waveFormat;
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = channels;
            waveFormat.nSamplesPerSec = sampleRate;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            if (FAILED(hr = primaryBuffer->SetFormat(&waveFormat)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to set DirectSound buffer format");

            sampleFormat = SampleFormat::SINT16;
            sampleSize = sizeof(int16_t);

            IDirectSoundBuffer* tempBuffer = nullptr;

            DSBUFFERDESC bufferDesc;
            bufferDesc.dwSize = sizeof(bufferDesc);
            bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY;
            bufferDesc.dwBufferBytes = 2 * bufferSize;
            bufferDesc.dwReserved = 0;
            bufferDesc.lpwfxFormat = &waveFormat;
            bufferDesc.guid3DAlgorithm = GUID_NULL;

            if (FAILED(hr = directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to create DirectSound buffer");

            if (FAILED(hr = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, reinterpret_cast<void**>(&buffer))))
            {
                tempBuffer->Release();
                throw std::system_error(hr, directSoundErrorCategory, "Failed to create DirectSound buffer");
            }

            if (FAILED(hr = tempBuffer->QueryInterface(IID_IDirectSoundNotify, reinterpret_cast<void**>(&notify))))
            {
                tempBuffer->Release();
                throw std::system_error(hr, directSoundErrorCategory, "Failed to get DirectSound notify interface");
            }

            tempBuffer->Release();

            uint8_t* bufferPointer;
            DWORD lockedBufferSize;
            if (FAILED(hr = buffer->Lock(0, bufferDesc.dwBufferBytes, reinterpret_cast<void**>(&bufferPointer), &lockedBufferSize, nullptr, 0, 0)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to lock DirectSound buffer");

            getData(lockedBufferSize / (channels * sizeof(int16_t)), data);
            std::copy(data.begin(), data.end(), bufferPointer);

            if (FAILED(hr = buffer->Unlock(bufferPointer, lockedBufferSize, nullptr, 0)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to unlock DirectSound buffer");

            nextBuffer = 0;

            notifyEvents[0] = CreateEvent(nullptr, true, false, nullptr);
            if (!notifyEvents[0])
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create event");

            notifyEvents[1] = CreateEvent(nullptr, true, false, nullptr);
            if (!notifyEvents[1])
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create event");

            DSBPOSITIONNOTIFY positionNotifyEvents[2];
            positionNotifyEvents[0].dwOffset = bufferSize - 1;
            positionNotifyEvents[0].hEventNotify = notifyEvents[0];

            positionNotifyEvents[1].dwOffset = 2 * bufferSize - 1;
            positionNotifyEvents[1].hEventNotify = notifyEvents[1];

            if (FAILED(hr = notify->SetNotificationPositions(2, positionNotifyEvents)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to set DirectSound notification positions");

            if (FAILED(hr = buffer->Play(0, 0, DSBPLAY_LOOPING)))
                throw std::system_error(hr, directSoundErrorCategory, "Failed to play DirectSound buffer");

            audioThread = std::thread(&AudioDeviceDS::run, this);
        }

        AudioDeviceDS::~AudioDeviceDS()
        {
            running = false;
            for (HANDLE notifyEvent : notifyEvents)
                SetEvent(notifyEvent);

            if (audioThread.joinable()) audioThread.join();

            for (HANDLE notifyEvent : notifyEvents)
                if (notifyEvent) CloseHandle(notifyEvent);

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
                    DWORD result;
                    if ((result = WaitForSingleObject(notifyEvents[nextBuffer], INFINITE)) == WAIT_FAILED)
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to wait for event");

                    if (result == WAIT_OBJECT_0)
                    {
                        if (!running) break;

                        if (!ResetEvent(notifyEvents[nextBuffer]))
                            throw std::system_error(GetLastError(), std::system_category(), "Failed to reset event");

                        process();

                        uint8_t* bufferPointer;
                        DWORD lockedBufferSize;
                        HRESULT hr;
                        if (FAILED(hr = buffer->Lock(nextBuffer * bufferSize, bufferSize, reinterpret_cast<void**>(&bufferPointer), &lockedBufferSize, nullptr, 0, 0)))
                            throw std::system_error(hr, directSoundErrorCategory, "Failed to lock DirectSound buffer");

                        getData(lockedBufferSize / (sampleSize * channels), data);

                        std::copy(data.begin(), data.end(), bufferPointer);

                        if (FAILED(hr = buffer->Unlock(bufferPointer, lockedBufferSize, nullptr, 0)))
                            throw std::system_error(hr, directSoundErrorCategory, "Failed to unlock DirectSound buffer");

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
