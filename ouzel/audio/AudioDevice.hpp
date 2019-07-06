// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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

            inline auto getDriver() const { return driver; }

            inline auto getAPIMajorVersion() const { return apiMajorVersion; }
            inline auto getAPIMinorVersion() const { return apiMinorVersion; }

            inline auto getBufferSize() const { return bufferSize; }
            inline auto getSampleRate() const { return sampleRate; }
            inline auto getChannels() const { return channels; }

            virtual void start() = 0;
            virtual void stop() = 0;

        private:
            Driver driver;

        protected:
            void getData(uint32_t frames, std::vector<uint8_t>& result);

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            SampleFormat sampleFormat = SampleFormat::SINT16;
            uint32_t bufferSize; // in frames
            uint32_t sampleRate;
            uint16_t channels;

        private:
            std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)> dataGetter;
            std::vector<float> buffer;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICE_HPP
