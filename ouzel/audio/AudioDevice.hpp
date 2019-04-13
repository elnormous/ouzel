// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICE_HPP
#define OUZEL_AUDIO_AUDIODEVICE_HPP

#include <functional>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/SampleFormat.hpp"
#include "utils/Inline.h"

namespace ouzel
{
    namespace audio
    {
        class AudioDevice
        {
        public:
            AudioDevice(Driver initDriver,
                        uint32_t initBufferSize,
                        uint32_t initSampleRate,
                        uint16_t initChannels, // zero for maximum available channels
                        const std::function<void(uint32_t frames,
                                                 uint16_t channels,
                                                 uint32_t sampleRate,
                                                 std::vector<float>& samples)>& initDataGetter);
            virtual ~AudioDevice();

            AudioDevice(const AudioDevice&) = delete;
            AudioDevice& operator=(const AudioDevice&) = delete;
            AudioDevice(AudioDevice&&) = delete;
            AudioDevice& operator=(AudioDevice&&) = delete;

            ALWAYSINLINE Driver getDriver() const { return driver; }

            ALWAYSINLINE uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            ALWAYSINLINE uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            ALWAYSINLINE uint32_t getBufferSize() const { return bufferSize; }
            ALWAYSINLINE uint32_t getSampleRate() const { return sampleRate; }
            ALWAYSINLINE uint16_t getChannels() const { return channels; }

            virtual void start() = 0;

        protected:
            void getData(uint32_t frames, std::vector<uint8_t>& result);

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            SampleFormat sampleFormat = SampleFormat::SINT16;
            uint32_t bufferSize; // in frames
            uint32_t sampleRate;
            uint16_t channels;

        private:
            Driver driver;
            std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)> dataGetter;
            std::vector<float> buffer;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICE_HPP
