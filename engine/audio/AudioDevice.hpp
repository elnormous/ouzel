// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICE_HPP
#define OUZEL_AUDIO_AUDIODEVICE_HPP

#include <functional>
#include <vector>
#include "Driver.hpp"
#include "SampleFormat.hpp"
#include "Settings.hpp"

namespace ouzel::audio
{
    class AudioDevice
    {
    public:
        AudioDevice(Driver initDriver,
                    const Settings& settings, // zero for maximum available channels
                    const std::function<void(std::uint32_t frames,
                                             std::uint32_t channels,
                                             std::uint32_t sampleRate,
                                             std::vector<float>& samples)>& initDataGetter);
        virtual ~AudioDevice() = default;

        AudioDevice(const AudioDevice&) = delete;
        AudioDevice& operator=(const AudioDevice&) = delete;
        AudioDevice(AudioDevice&&) = delete;
        AudioDevice& operator=(AudioDevice&&) = delete;

        auto getDriver() const noexcept { return driver; }

        auto getAPIMajorVersion() const noexcept { return apiMajorVersion; }
        auto getAPIMinorVersion() const noexcept { return apiMinorVersion; }

        auto getBufferSize() const noexcept { return bufferSize; }
        auto getSampleRate() const noexcept { return sampleRate; }
        auto getChannels() const noexcept { return channels; }

        virtual void start() = 0;
        virtual void stop() = 0;

    private:
        Driver driver;

    protected:
        void getData(std::uint32_t frames, std::vector<std::uint8_t>& result);

        std::uint16_t apiMajorVersion = 0;
        std::uint16_t apiMinorVersion = 0;

        SampleFormat sampleFormat = SampleFormat::signedInt16;
        std::uint32_t bufferSize; // in frames
        std::uint32_t sampleRate;
        std::uint32_t channels;

    private:
        std::function<void(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples)> dataGetter;
        std::vector<float> buffer;
    };
}

#endif // OUZEL_AUDIO_AUDIODEVICE_HPP
