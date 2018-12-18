// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <string>
#include <Audioclient.h>
#include <mmdeviceapi.h>
#include "AudioDeviceWASAPI.hpp"

namespace ouzel
{
    namespace audio
    {
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
            IMMDeviceEnumerator* enumerator = nullptr;

            HRESULT hr;
            if (FAILED(hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<LPVOID*>(&enumerator))))
                throw std::system_error(hr, wasapiErrorCategory, "Failed to create device enumerator");

            IMMDevice* device;
            if (FAILED(hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device)))
                throw std::system_error(hr, wasapiErrorCategory, "Failed to get audio endpoint");

            if (enumerator) enumerator->Release();
        }
    } // namespace audio
} // namespace ouzel
