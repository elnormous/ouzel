// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "core/Setup.h"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Listener.hpp"
#include "alsa/AudioDeviceALSA.hpp"
#include "core/Engine.hpp"
#include "coreaudio/AudioDeviceCA.hpp"
#include "dsound/AudioDeviceDS.hpp"
#include "empty/AudioDeviceEmpty.hpp"
#include "openal/AudioDeviceAL.hpp"
#include "opensl/AudioDeviceSL.hpp"
#include "xaudio2/AudioDeviceXA2.hpp"
#include "math/MathUtils.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        class Destination final: public Node
        {
        public:
            Destination()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
                Node::process(samples, channels, sampleRate, position);
            }
        };

        std::set<Driver> Audio::getAvailableAudioDrivers()
        {
            static std::set<Driver> availableDrivers;

            if (availableDrivers.empty())
            {
                availableDrivers.insert(Driver::EMPTY);

#if OUZEL_COMPILE_OPENAL
                availableDrivers.insert(Driver::OPENAL);
#endif

#if OUZEL_COMPILE_DIRECTSOUND
                availableDrivers.insert(Driver::DIRECTSOUND);
#endif

#if OUZEL_COMPILE_XAUDIO2
                availableDrivers.insert(Driver::XAUDIO2);
#endif

#if OUZEL_COMPILE_OPENSL
                availableDrivers.insert(Driver::OPENSL);
#endif

#if OUZEL_COMPILE_COREAUDIO
                availableDrivers.insert(Driver::COREAUDIO);
#endif

#if OUZEL_COMPILE_ALSA
                availableDrivers.insert(Driver::ALSA);
#endif
            }

            return availableDrivers;
        }

        Audio::Audio(Driver driver, bool debugAudio, Window* window)
        {
            switch (driver)
            {
#if OUZEL_COMPILE_OPENAL
                case Driver::OPENAL:
                    engine->log(Log::Level::INFO) << "Using OpenAL audio driver";
                    device.reset(new AudioDeviceAL());
                    break;
#endif
#if OUZEL_COMPILE_DIRECTSOUND
                case Driver::DIRECTSOUND:
                    engine->log(Log::Level::INFO) << "Using DirectSound audio driver";
                    device.reset(new AudioDeviceDS(window));
                    break;
#endif
#if OUZEL_COMPILE_XAUDIO2
                case Driver::XAUDIO2:
                    engine->log(Log::Level::INFO) << "Using XAudio 2 audio driver";
                    device.reset(new AudioDeviceXA2(debugAudio));
                    break;
#endif
#if OUZEL_COMPILE_OPENSL
                case Driver::OPENSL:
                    engine->log(Log::Level::INFO) << "Using OpenSL ES audio driver";
                    device.reset(new AudioDeviceSL());
                    break;
#endif
#if OUZEL_COMPILE_COREAUDIO
                case Driver::COREAUDIO:
                    engine->log(Log::Level::INFO) << "Using CoreAudio audio driver";
                    device.reset(new AudioDeviceCA());
                    break;
#endif
#if OUZEL_COMPILE_ALSA
                case Driver::ALSA:
                    engine->log(Log::Level::INFO) << "Using ALSA audio driver";
                    device.reset(new AudioDeviceALSA());
                    break;
#endif
                default:
                    engine->log(Log::Level::INFO) << "Not using audio driver";
                    device.reset(new AudioDeviceEmpty());
                    (void)debugAudio;
                    (void)window;
                    break;
            }

            sinkNodeId = initNode([]() { return std::unique_ptr<Node>(new Destination()); });

            AudioDevice::Command command(AudioDevice::Command::Type::SET_DESTINATION_NODE);
            command.nodeId = sinkNodeId;
            device->addCommand(command);
        }

        Audio::~Audio()
        {
        }

        uintptr_t Audio::initNode(const std::function<std::unique_ptr<Node>(void)>& createFunction)
        {
            uintptr_t nodeId = device->getNodeId();
            AudioDevice::Command command(AudioDevice::Command::Type::INIT_NODE);
            command.nodeId = nodeId;
            command.createFunction = createFunction;
            device->addCommand(command);
            return nodeId;
        }

        void Audio::deleteNode(uintptr_t nodeId)
        {
            AudioDevice::Command command(AudioDevice::Command::Type::DELETE_NODE);
            command.nodeId = nodeId;
            device->addCommand(command);
        }

        void Audio::updateNode(uintptr_t nodeId, const std::function<void(Node*)>& updateFunction)
        {
            AudioDevice::Command command(AudioDevice::Command::Type::UPDATE_NODE);
            command.nodeId = nodeId;
            command.updateFunction = updateFunction;
            device->addCommand(command);
        }
    } // namespace audio
} // namespace ouzel
