// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIO_HPP
#define OUZEL_AUDIO_AUDIO_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/Mix.hpp"
#include "audio/mixer/Processor.hpp"
#include "audio/mixer/Mixer.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    class Window;

    namespace audio
    {
        class AudioDevice;
        class Listener;

        class Audio final
        {
        public:
            Audio(Driver driver, bool debugAudio, Window* window);
            ~Audio();

            Audio(const Audio&) = delete;
            Audio& operator=(const Audio&) = delete;
            Audio(Audio&&) = delete;
            Audio& operator=(Audio&&) = delete;

            static Driver getDriver(const std::string& driver);
            static std::set<Driver> getAvailableAudioDrivers();

            inline AudioDevice* getDevice() const { return device.get(); }
            inline mixer::Mixer& getMixer() { return mixer; }

            void update();

            void deleteObject(uintptr_t objectId);
            uintptr_t initBus();
            uintptr_t initStream(uintptr_t sourceId);
            uintptr_t initSource(const std::function<std::unique_ptr<mixer::Source>()>& initFunction);
            uintptr_t initProcessor(std::unique_ptr<mixer::Processor>&& processor);
            void updateProcessor(uintptr_t processorId, const std::function<void(mixer::Processor*)>& updateFunction);

            Mix& getMasterMix() { return masterMix; }

        private:
            void eventCallback(const mixer::Mixer::Event& event);

            mixer::Mixer mixer;
            Mix masterMix;
            std::unique_ptr<AudioDevice> device;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIO_HPP
