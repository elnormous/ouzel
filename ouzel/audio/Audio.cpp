// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "core/Setup.h"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Listener.hpp"
#include "alsa/ALSAAudioDevice.hpp"
#include "core/Engine.hpp"
#include "coreaudio/CAAudioDevice.hpp"
#include "dsound/DSAudioDevice.hpp"
#include "empty/EmptyAudioDevice.hpp"
#include "openal/OALAudioDevice.hpp"
#include "opensl/OSLAudioDevice.hpp"
#include "xaudio2/XA2AudioDevice.hpp"
#include "wasapi/WASAPIAudioDevice.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        Driver Audio::getDriver(const std::string& driver)
        {
            if (driver.empty() || driver == "default")
            {
                auto availableDrivers = Audio::getAvailableAudioDrivers();

                if (availableDrivers.find(Driver::WASAPI) != availableDrivers.end())
                    return Driver::WASAPI;
                else if (availableDrivers.find(Driver::CoreAudio) != availableDrivers.end())
                    return Driver::CoreAudio;
                else if (availableDrivers.find(Driver::ALSA) != availableDrivers.end())
                    return Driver::ALSA;
                else if (availableDrivers.find(Driver::OpenAL) != availableDrivers.end())
                    return Driver::OpenAL;
                else if (availableDrivers.find(Driver::XAudio2) != availableDrivers.end())
                    return Driver::XAudio2;
                else if (availableDrivers.find(Driver::DirectSound) != availableDrivers.end())
                    return Driver::DirectSound;
                else if (availableDrivers.find(Driver::OpenSL) != availableDrivers.end())
                    return Driver::OpenSL;
                else
                    return Driver::Empty;
            }
            else if (driver == "empty")
                return Driver::Empty;
            else if (driver == "openal")
                return Driver::OpenAL;
            else if (driver == "directsound")
                return Driver::DirectSound;
            else if (driver == "xaudio2")
                return Driver::XAudio2;
            else if (driver == "opensl")
                return Driver::OpenSL;
            else if (driver == "coreaudio")
                return Driver::CoreAudio;
            else if (driver == "alsa")
                return Driver::ALSA;
            else if (driver == "wasapi")
                return Driver::WASAPI;
            else
                throw std::runtime_error("Invalid audio driver");
        }

        std::set<Driver> Audio::getAvailableAudioDrivers()
        {
            static std::set<Driver> availableDrivers;

            if (availableDrivers.empty())
            {
                availableDrivers.insert(Driver::Empty);

#if OUZEL_COMPILE_OPENAL
                availableDrivers.insert(Driver::OpenAL);
#endif
#if OUZEL_COMPILE_DIRECTSOUND
                availableDrivers.insert(Driver::DirectSound);
#endif
#if OUZEL_COMPILE_XAUDIO2
                availableDrivers.insert(Driver::XAudio2);
#endif
#if OUZEL_COMPILE_OPENSL
                availableDrivers.insert(Driver::OpenSL);
#endif
#if OUZEL_COMPILE_COREAUDIO
                availableDrivers.insert(Driver::CoreAudio);
#endif
#if OUZEL_COMPILE_ALSA
                availableDrivers.insert(Driver::ALSA);
#endif
#if OUZEL_COMPILE_WASAPI
                availableDrivers.insert(Driver::WASAPI);
#endif
            }

            return availableDrivers;
        }

        static std::unique_ptr<AudioDevice> createAudioDevice(Driver driver,
                                                              const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& dataGetter,
                                                              bool debugAudio)
        {
            switch (driver)
            {
#if OUZEL_COMPILE_OPENAL
                case Driver::OpenAL:
                    engine->log(Log::Level::Info) << "Using OpenAL audio driver";
                    return std::make_unique<openal::AudioDevice>(512, 44100, 0, dataGetter);
#endif
#if OUZEL_COMPILE_DIRECTSOUND
                case Driver::DirectSound:
                    engine->log(Log::Level::Info) << "Using DirectSound audio driver";
                    return std::make_unique<directsound::AudioDevice>(512, 44100, 0, dataGetter);
#endif
#if OUZEL_COMPILE_XAUDIO2
                case Driver::XAudio2:
                    engine->log(Log::Level::Info) << "Using XAudio 2 audio driver";
                    return std::make_unique<xaudio2::AudioDevice>(512, 44100, 0, dataGetter, debugAudio);
#endif
#if OUZEL_COMPILE_OPENSL
                case Driver::OpenSL:
                    engine->log(Log::Level::Info) << "Using OpenSL ES audio driver";
                    return std::make_unique<opensl::AudioDevice>(512, 44100, 0, dataGetter);
#endif
#if OUZEL_COMPILE_COREAUDIO
                case Driver::CoreAudio:
                    engine->log(Log::Level::Info) << "Using CoreAudio audio driver";
                    return std::make_unique<coreaudio::AudioDevice>(512, 44100, 0, dataGetter);
#endif
#if OUZEL_COMPILE_ALSA
                case Driver::ALSA:
                    engine->log(Log::Level::Info) << "Using ALSA audio driver";
                    return std::make_unique<alsa::AudioDevice>(512, 44100, 0, dataGetter);
#endif
#if OUZEL_COMPILE_WASAPI
                case Driver::WASAPI:
                    engine->log(Log::Level::Info) << "Using WASAPI audio driver";
                    return std::make_unique<wasapi::AudioDevice>(512, 44100, 0, dataGetter);
#endif
                default:
                    engine->log(Log::Level::Info) << "Not using audio driver";
                    static_cast<void>(debugAudio);
                    return std::make_unique<empty::AudioDevice>(512, 44100, 0, dataGetter);
            }
        }

        Audio::Audio(Driver driver, bool debugAudio):
            device(createAudioDevice(driver,
                                     std::bind(&Audio::getSamples, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                                     debugAudio)),
            mixer(device->getBufferSize(), device->getChannels(),
                  std::bind(&Audio::eventCallback, this, std::placeholders::_1)),
            masterMix(*this),
            rootNode(*this) // mixer.getRootObjectId()
        {
            addCommand(std::make_unique<mixer::SetMasterBusCommand>(masterMix.getBusId()));
            device->start();
        }

        Audio::~Audio()
        {
        }

        void Audio::update()
        {
            // TODO: handle events from the audio device

            mixer.submitCommandBuffer(std::move(commandBuffer));
            commandBuffer = mixer::CommandBuffer();
        }

        void Audio::deleteObject(uintptr_t objectId)
        {
            addCommand(std::make_unique<mixer::DeleteObjectCommand>(objectId));
        }

        uintptr_t Audio::initBus()
        {
            uintptr_t busId = mixer.getObjectId();
            addCommand(std::make_unique<mixer::InitBusCommand>(busId));
            return busId;
        }

        uintptr_t Audio::initStream(uintptr_t sourceId)
        {
            uintptr_t streamId = mixer.getObjectId();
            addCommand(std::make_unique<mixer::InitStreamCommand>(streamId, sourceId));
            return streamId;
        }

        uintptr_t Audio::initData(std::unique_ptr<mixer::Data> data)
        {
            uintptr_t dataId = mixer.getObjectId();
            addCommand(std::make_unique<mixer::InitDataCommand>(dataId, std::move(data)));
            return dataId;
        }

        uintptr_t Audio::initProcessor(std::unique_ptr<mixer::Processor> processor)
        {
            uintptr_t processorId = mixer.getObjectId();
            addCommand(std::make_unique<mixer::InitProcessorCommand>(processorId, std::move(processor)));
            return processorId;
        }

        void Audio::updateProcessor(uintptr_t processorId, const std::function<void(mixer::Processor*)>& updateFunction)
        {
            addCommand(std::make_unique<mixer::UpdateProcessorCommand>(processorId, updateFunction));
        }

        void Audio::getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)
        {
            mixer.getSamples(frames, channels, sampleRate, samples);
        }

        void Audio::eventCallback(const mixer::Mixer::Event& event)
        {
        }
    } // namespace audio
} // namespace ouzel
