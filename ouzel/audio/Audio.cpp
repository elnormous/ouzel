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
        class Sink final: public Node
        {
        public:
            Sink()
            {
            }

            void process(std::vector<float>& samples, uint16_t&,
                         uint32_t&, Vector3&) override
            {
                std::fill(samples.begin(), samples.end(), 0.0F);
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

            sinkNodeId = initNode([]() { return std::unique_ptr<Node>(new Sink()); });

            AudioDevice::Command command(AudioDevice::Command::Type::SET_OUTPUT_NODE);
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

        void Audio::resample(const std::vector<float>& src, uint32_t srcFrames,
                             std::vector<float>& dst, uint32_t dstFrames,
                             uint32_t channels)
        {
            if (dstFrames > 0) // do resampling only if destination is not empty
            {
                if (srcFrames == 0) // source is empty
                {
                    dst.resize(dstFrames * channels);

                    std::fill(dst.begin(), dst.end(), 0.0F);
                }
                else
                {
                    float srcIncrement = static_cast<float>(srcFrames - 1) / static_cast<float>(dstFrames - 1);
                    float srcPosition = 0.0F;

                    dst.resize(dstFrames * channels);

                    for (uint32_t frame = 0; frame < dstFrames - 1; ++frame)
                    {
                        uint32_t srcCurrentFrame = static_cast<uint32_t>(srcPosition);
                        float frac = srcPosition - srcCurrentFrame;

                        srcPosition += srcIncrement;
                        uint32_t srcNextFrame = srcCurrentFrame + 1;

                        for (uint32_t channel = 0; channel < channels; ++channel)
                        {
                            uint32_t srcCurrentPosition = srcCurrentFrame * channels + channel;
                            uint32_t srcNextPosition = srcNextFrame * channels + channel;

                            dst[frame * channels + channel] = ouzel::lerp(src[srcCurrentPosition], src[srcNextPosition], frac);
                        }
                    }

                    // fill the last frame of the destination with the last frame of the source
                    for (uint32_t channel = 0; channel < channels; ++channel)
                        dst[(dstFrames - 1) * channels + channel] = src[(srcFrames - 1) * channels + channel];
                }
            }
        }
    } // namespace audio
} // namespace ouzel
