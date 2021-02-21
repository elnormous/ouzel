// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_EMPTYAUDIODEVICE_HPP
#define OUZEL_AUDIO_EMPTYAUDIODEVICE_HPP

#include "../AudioDevice.hpp"

namespace ouzel::audio::empty
{
    class AudioDevice final: public audio::AudioDevice
    {
    public:
        AudioDevice(const Settings& settings,
                    const std::function<void(std::uint32_t frames,
                                             std::uint32_t channels,
                                             std::uint32_t sampleRate,
                                             std::vector<float>& samples)>& initDataGetter):
            audio::AudioDevice(Driver::empty, settings, initDataGetter)
        {
        }

        void start() final {}
        void stop() final {}
    };
}

#endif // OUZEL_AUDIO_EMPTYAUDIODEVICE_HPP
