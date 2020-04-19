// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIO_HPP
#define OUZEL_AUDIO_AUDIO_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "AudioDevice.hpp"
#include "Driver.hpp"
#include "Mix.hpp"
#include "Node.hpp"
#include "mixer/Commands.hpp"
#include "mixer/Processor.hpp"
#include "mixer/Mixer.hpp"
#include "../math/Quaternion.hpp"
#include "../math/Vector.hpp"

namespace ouzel
{
    namespace audio
    {
        class Listener;

        class Audio final
        {
        public:
            Audio(Driver driver, bool debugAudio);

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

            void deleteObject(std::uintptr_t objectId);
            std::uintptr_t initBus();
            std::uintptr_t initStream(std::uintptr_t sourceId);
            std::uintptr_t initData(std::unique_ptr<mixer::Data> data);
            std::uintptr_t initProcessor(std::unique_ptr<mixer::Processor> processor);
            void updateProcessor(std::uintptr_t processorId, const std::function<void(mixer::Processor*)>& updateFunction);

            auto& getRootNode() { return rootNode; }

        private:
            void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples);
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
