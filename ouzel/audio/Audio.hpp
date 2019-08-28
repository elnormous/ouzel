// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIO_HPP
#define OUZEL_AUDIO_AUDIO_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/Mix.hpp"
#include "audio/Node.hpp"
#include "audio/mixer/Commands.hpp"
#include "audio/mixer/Processor.hpp"
#include "audio/mixer/Mixer.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDevice;
        class Listener;

        class Audio final
        {
        public:
            Audio(Driver driver, bool debugAudio);
            ~Audio();

            Audio(const Audio&) = delete;
            Audio& operator=(const Audio&) = delete;
            Audio(Audio&&) = delete;
            Audio& operator=(Audio&&) = delete;

            static Driver getDriver(const std::string& driver);
            static std::set<Driver> getAvailableAudioDrivers();

            inline auto getDevice() const noexcept { return device.get(); }
            inline mixer::Mixer& getMixer() { return mixer; }
            inline Mix& getMasterMix() { return masterMix; }

            void update();

            inline void addCommand(std::unique_ptr<mixer::Command> command)
            {
                commandBuffer.pushCommand(std::move(command));
            }

            void deleteObject(uintptr_t objectId);
            uintptr_t initBus(std::unique_ptr<mixer::Processor> processor);
            uintptr_t initStream(uintptr_t sourceId);
            uintptr_t initData(std::unique_ptr<mixer::Data> data);

            auto& getRootNode() { return rootNode; }

        private:
            void getSamples(uint32_t frames, uint32_t channels, uint32_t sampleRate, std::vector<float>& samples);
            void eventCallback(const mixer::Mixer::Event& event);

            std::unique_ptr<AudioDevice> device;
            mixer::Mixer mixer;
            mixer::CommandBuffer commandBuffer;
            Mix masterMix;
            Node rootNode;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIO_HPP
