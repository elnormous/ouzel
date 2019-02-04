// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICE_HPP
#define OUZEL_AUDIO_AUDIODEVICE_HPP

#include <functional>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/SampleFormat.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDevice
        {
        public:
            AudioDevice(Driver initDriver,
                        const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter);
            virtual ~AudioDevice();

            AudioDevice(const AudioDevice&) = delete;
            AudioDevice& operator=(const AudioDevice&) = delete;
            AudioDevice(AudioDevice&&) = delete;
            AudioDevice& operator=(AudioDevice&&) = delete;

            inline uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            inline uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            virtual void process();

        protected:
            void getData(uint32_t frames, std::vector<uint8_t>& result);

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            SampleFormat sampleFormat = SampleFormat::SINT16;
            uint32_t bufferSize = 2 * 4096;
            uint32_t sampleRate = 44100;
            uint16_t channels = 2;

        private:
            std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)> dataGetter;
            std::vector<float> buffer;

            Driver driver;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICE_HPP
