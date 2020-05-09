// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

#include <system_error>
#include "DSAudioDevice.hpp"
#include "../../core/Engine.hpp"
#include "../../core/Window.hpp"

#ifndef WAVE_FORMAT_IEEE_FLOAT
#  define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

namespace
{
    BOOL CALLBACK enumCallback(LPGUID, LPCWSTR description, LPCWSTR, LPVOID)
    {
        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, description, -1, nullptr, 0, nullptr, nullptr);
        if (bufferSize != 0)
        {
            std::vector<char> buffer(bufferSize);
            if (WideCharToMultiByte(CP_UTF8, 0, description, -1, buffer.data(), bufferSize, nullptr, nullptr) != 0)
                ouzel::engine->log(ouzel::Log::Level::info) << "Using " << buffer.data() << " for audio";
        }
        return FALSE;
    }
}

namespace ouzel
{
    namespace audio
    {
        namespace directsound
        {
            namespace
            {
                class ErrorCategory final: public std::error_category
                {
                public:
                    const char* name() const noexcept final
                    {
                        return "DirectSound";
                    }

                    std::string message(int condition) const final
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

                const ErrorCategory errorCategory {};
            }

            AudioDevice::AudioDevice(std::uint32_t initBufferSize,
                                     std::uint32_t initSampleRate,
                                     std::uint32_t initChannels,
                                     const std::function<void(std::uint32_t frames,
                                                              std::uint32_t channels,
                                                              std::uint32_t sampleRate,
                                                              std::vector<float>& samples)>& initDataGetter):
                audio::AudioDevice(Driver::DirectSound, initBufferSize, initSampleRate, initChannels, initDataGetter)
            {
                HRESULT hr;
                if (FAILED(hr = DirectSoundEnumerateW(enumCallback, this)))
                    throw std::system_error(hr, errorCategory, "Failed to enumerate DirectSound 8 devices");

                IDirectSound8* directSoundPointer;
                if (FAILED(hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &directSoundPointer, nullptr)))
                    throw std::system_error(hr, errorCategory, "Failed to create DirectSound 8 instance");

                directSound = directSoundPointer;

                if (FAILED(hr = directSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY)))
                    throw std::system_error(hr, errorCategory, "Failed to set cooperative level for DirectSound 8");

                DSBUFFERDESC primaryBufferDesc;
                primaryBufferDesc.dwSize = sizeof(primaryBufferDesc);
                primaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
                primaryBufferDesc.dwBufferBytes = 0;
                primaryBufferDesc.dwReserved = 0;
                primaryBufferDesc.lpwfxFormat = nullptr;
                primaryBufferDesc.guid3DAlgorithm = GUID_NULL;

                IDirectSoundBuffer* primaryBufferPointer;
                if (FAILED(hr = directSound->CreateSoundBuffer(&primaryBufferDesc, &primaryBufferPointer, nullptr)))
                    throw std::system_error(hr, errorCategory, "Failed to create DirectSound buffer");

                primaryBuffer = primaryBufferPointer;

                WAVEFORMATEX waveFormat;
                waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
                waveFormat.nChannels = static_cast<WORD>(channels);
                waveFormat.nSamplesPerSec = sampleRate;
                waveFormat.wBitsPerSample = sizeof(float) * 8;
                waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
                waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
                waveFormat.cbSize = 0;

                sampleFormat = SampleFormat::Float32;
                sampleSize = sizeof(float);

                if (FAILED(hr = primaryBuffer->SetFormat(&waveFormat)))
                {
                    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
                    waveFormat.wBitsPerSample = sizeof(std::int16_t) * 8;
                    waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
                    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

                    if (FAILED(hr = primaryBuffer->SetFormat(&waveFormat)))
                        throw std::system_error(hr, errorCategory, "Failed to set DirectSound buffer format");

                    sampleFormat = SampleFormat::SignedInt16;
                    sampleSize = sizeof(std::int16_t);
                }

                DSBUFFERDESC bufferDesc;
                bufferDesc.dwSize = sizeof(bufferDesc);
                bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY;
                bufferDesc.dwBufferBytes = 2 * bufferSize;
                bufferDesc.dwReserved = 0;
                bufferDesc.lpwfxFormat = &waveFormat;
                bufferDesc.guid3DAlgorithm = GUID_NULL;

                IDirectSoundBuffer* tempBufferPointer;
                if (FAILED(hr = directSound->CreateSoundBuffer(&bufferDesc, &tempBufferPointer, nullptr)))
                    throw std::system_error(hr, errorCategory, "Failed to create DirectSound buffer");

                Pointer<IDirectSoundBuffer> tempBuffer = tempBufferPointer;

                void* bufferPointer;
                if (FAILED(hr = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, &bufferPointer)))
                    throw std::system_error(hr, errorCategory, "Failed to create DirectSound buffer");

                buffer = static_cast<IDirectSoundBuffer8*>(bufferPointer);

                void* notifyPointer;
                if (FAILED(hr = tempBuffer->QueryInterface(IID_IDirectSoundNotify, &notifyPointer)))
                    throw std::system_error(hr, errorCategory, "Failed to get DirectSound notify interface");

                notify = static_cast<IDirectSoundNotify*>(notifyPointer);

                void* bufferDataPointer;
                DWORD lockedBufferSize;
                if (FAILED(hr = buffer->Lock(0, bufferDesc.dwBufferBytes, &bufferDataPointer, &lockedBufferSize, nullptr, 0, 0)))
                    throw std::system_error(hr, errorCategory, "Failed to lock DirectSound buffer");

                std::uint8_t* bufferData = static_cast<std::uint8_t*>(bufferDataPointer);

                getData(lockedBufferSize / (channels * sizeof(std::int16_t)), data);
                std::copy(data.begin(), data.end(), bufferData);

                if (FAILED(hr = buffer->Unlock(bufferDataPointer, lockedBufferSize, nullptr, 0)))
                    throw std::system_error(hr, errorCategory, "Failed to unlock DirectSound buffer");

                nextBuffer = 0;

                notifyEvents[0] = CreateEvent(nullptr, false, false, nullptr);
                if (!notifyEvents[0])
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create event");

                notifyEvents[1] = CreateEvent(nullptr, false, false, nullptr);
                if (!notifyEvents[1])
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create event");

                DSBPOSITIONNOTIFY positionNotifyEvents[2];
                positionNotifyEvents[0].dwOffset = bufferSize - 1;
                positionNotifyEvents[0].hEventNotify = notifyEvents[0];

                positionNotifyEvents[1].dwOffset = 2 * bufferSize - 1;
                positionNotifyEvents[1].hEventNotify = notifyEvents[1];

                if (FAILED(hr = notify->SetNotificationPositions(2, positionNotifyEvents)))
                    throw std::system_error(hr, errorCategory, "Failed to set DirectSound notification positions");
            }

            AudioDevice::~AudioDevice()
            {
                running = false;
                for (HANDLE notifyEvent : notifyEvents)
                    if (notifyEvent) SetEvent(notifyEvent);

                if (audioThread.isJoinable()) audioThread.join();

                for (HANDLE notifyEvent : notifyEvents)
                    if (notifyEvent) CloseHandle(notifyEvent);
            }

            void AudioDevice::start()
            {
                HRESULT hr;
                if (FAILED(hr = buffer->Play(0, 0, DSBPLAY_LOOPING)))
                    throw std::system_error(hr, errorCategory, "Failed to play DirectSound buffer");

                running = true;
                audioThread = Thread(&AudioDevice::run, this);
            }

            void AudioDevice::stop()
            {
                running = false;
                if (audioThread.isJoinable()) audioThread.join();

                HRESULT hr;
                if (FAILED(hr = buffer->Stop()))
                    throw std::system_error(hr, errorCategory, "Failed to stop DirectSound buffer");
            }

            void AudioDevice::run()
            {
                Thread::setCurrentThreadName("Audio");

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

                            void* bufferDataPointer;
                            DWORD lockedBufferSize;
                            HRESULT hr;
                            if (FAILED(hr = buffer->Lock(nextBuffer * bufferSize, bufferSize, &bufferDataPointer, &lockedBufferSize, nullptr, 0, 0)))
                                throw std::system_error(hr, errorCategory, "Failed to lock DirectSound buffer");

                            std::uint8_t* bufferData = static_cast<std::uint8_t*>(bufferDataPointer);

                            getData(lockedBufferSize / (sampleSize * channels), data);

                            std::copy(data.begin(), data.end(), bufferData);

                            if (FAILED(hr = buffer->Unlock(bufferDataPointer, lockedBufferSize, nullptr, 0)))
                                throw std::system_error(hr, errorCategory, "Failed to unlock DirectSound buffer");

                            nextBuffer = (nextBuffer + 1) % 2;
                        }
                    }
                    catch (const std::exception& e)
                    {
                        engine->log(Log::Level::error) << e.what();
                    }
                }
            }
        } // namespace directsound
    } // namespace audio
} // namespace ouzel

#endif
