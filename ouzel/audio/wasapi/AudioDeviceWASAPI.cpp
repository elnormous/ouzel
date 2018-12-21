// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_WASAPI

#include <string>
#include "AudioDeviceWASAPI.hpp"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

namespace ouzel
{
    namespace audio
    {
        class NotificationClient: public IMMNotificationClient
        {
            LONG refCount = 1;

        public:
            NotificationClient() {}
            NotificationClient(const NotificationClient&) = delete;
            NotificationClient(NotificationClient&&) = delete;
            NotificationClient& operator=(const NotificationClient&) = delete;
            NotificationClient& operator=(NotificationClient&&) = delete;

            ULONG STDMETHODCALLTYPE AddRef()
            {
                return InterlockedIncrement(&refCount);
            }

            ULONG STDMETHODCALLTYPE Release()
            {
                ULONG newRefCount = InterlockedDecrement(&refCount);
                if (!newRefCount)
                    delete this;

                return newRefCount;
            }

            HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface)
            {
                if (riid == IID_IUnknown)
                {
                    AddRef();
                    *ppvInterface = this;
                }
                else if (riid == __uuidof(IMMNotificationClient))
                {
                    AddRef();
                    *ppvInterface = this;
                }
                else
                {
                    *ppvInterface = nullptr;
                    return E_NOINTERFACE;
                }
                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId)
            {
                return S_OK;
            };

            HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId)
            {
                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState)
            {
                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId)
            {
                if (role == eConsole)
                {
                    if (flow == eRender)
                    {
                        // TODO: implement
                    }
                    else if (flow == eCapture)
                    {
                        // TODO: implement
                    }
                }

                return S_OK;
            }

            HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
            {
                return S_OK;
            }
        };

        class WASAPIErrorCategory : public std::error_category
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
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        const WASAPIErrorCategory wasapiErrorCategory{};

        AudioDeviceWASAPI::AudioDeviceWASAPI(Mixer& initMixer):
            AudioDevice(Driver::WASAPI, initMixer)
        {
            HRESULT hr;
            if (FAILED(hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<LPVOID*>(&enumerator))))
                throw std::system_error(hr, wasapiErrorCategory, "Failed to create device enumerator");

            if (FAILED(hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device)))
                throw std::system_error(hr, wasapiErrorCategory, "Failed to get audio endpoint");

            notificationClient = new NotificationClient();

            if (FAILED(hr = enumerator->RegisterEndpointNotificationCallback(notificationClient)))
                throw std::system_error(hr, wasapiErrorCategory, "Failed to get audio endpoint");

            if (FAILED(hr = device->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&audioClient))))
                throw std::system_error(hr, wasapiErrorCategory, "Failed to activate audio device");

            WAVEFORMATEX* audioClientWaveFormat;

            if (FAILED(hr = audioClient->GetMixFormat(&audioClientWaveFormat)))
                throw std::system_error(hr, wasapiErrorCategory, "Failed to get audio mix format");

            WAVEFORMATEX waveFormat;
            waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
            waveFormat.nChannels = channels;
            waveFormat.nSamplesPerSec = sampleRate;
            waveFormat.wBitsPerSample = 32;
            waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            DWORD streamFlags = 0;

            if (waveFormat.nSamplesPerSec != audioClientWaveFormat->nSamplesPerSec)
                streamFlags |= AUDCLNT_STREAMFLAGS_RATEADJUST;

            CoTaskMemFree(audioClientWaveFormat);

            if (FAILED(hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, 0, 0, &waveFormat, nullptr)))
            {
                waveFormat.wFormatTag = WAVE_FORMAT_PCM;
                waveFormat.wBitsPerSample = 16;

                if (FAILED(hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, 0, 0, &waveFormat, nullptr)))
                    throw std::system_error(hr, wasapiErrorCategory, "Failed to initialize audio client");

                sampleFormat = SampleFormat::SINT16;
                sampleSize = sizeof(int16_t);
            }
            else
            {
                sampleFormat = SampleFormat::FLOAT32;
                sampleSize = sizeof(float);
            }
        }

        AudioDeviceWASAPI::~AudioDeviceWASAPI()
        {
            if (renderClient) renderClient->Release();
            if (audioClient) audioClient->Release();
            if (notificationClient) notificationClient->Release();
            if (device) device->Release();
            if (enumerator) enumerator->Release();
        }
    } // namespace audio
} // namespace ouzel

#endif
