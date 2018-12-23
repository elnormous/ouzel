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
#include "wasapi/AudioDeviceWASAPI.hpp"
#include "math/MathUtils.hpp"
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

                if (availableDrivers.find(Driver::COREAUDIO) != availableDrivers.end())
                    return Driver::COREAUDIO;
                else if (availableDrivers.find(Driver::ALSA) != availableDrivers.end())
                    return Driver::ALSA;
                else if (availableDrivers.find(Driver::OPENAL) != availableDrivers.end())
                    return Driver::OPENAL;
                else if (availableDrivers.find(Driver::XAUDIO2) != availableDrivers.end())
                    return Driver::XAUDIO2;
                else if (availableDrivers.find(Driver::DIRECTSOUND) != availableDrivers.end())
                    return Driver::DIRECTSOUND;
                else if (availableDrivers.find(Driver::OPENSL) != availableDrivers.end())
                    return Driver::OPENSL;
                else
                    return Driver::EMPTY;
            }
            else if (driver == "empty")
                return Driver::EMPTY;
            else if (driver == "openal")
                return Driver::OPENAL;
            else if (driver == "directsound")
                return Driver::DIRECTSOUND;
            else if (driver == "xaudio2")
                return Driver::XAUDIO2;
            else if (driver == "opensl")
                return Driver::OPENSL;
            else if (driver == "coreaudio")
                return Driver::COREAUDIO;
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
#if OUZEL_COMPILE_WASAPI
                availableDrivers.insert(Driver::WASAPI);
#endif
            }

            return availableDrivers;
        }

        static std::unique_ptr<AudioDevice> createAudioDevice(Driver driver, Mixer& mixer, bool debugAudio, Window* window)
        {
            switch (driver)
            {
#if OUZEL_COMPILE_OPENAL
                case Driver::OPENAL:
                    engine->log(Log::Level::INFO) << "Using OpenAL audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceAL(mixer));
#endif
#if OUZEL_COMPILE_DIRECTSOUND
                case Driver::DIRECTSOUND:
                    engine->log(Log::Level::INFO) << "Using DirectSound audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceDS(mixer, window));
#endif
#if OUZEL_COMPILE_XAUDIO2
                case Driver::XAUDIO2:
                    engine->log(Log::Level::INFO) << "Using XAudio 2 audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceXA2(mixer, debugAudio));
#endif
#if OUZEL_COMPILE_OPENSL
                case Driver::OPENSL:
                    engine->log(Log::Level::INFO) << "Using OpenSL ES audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceSL(mixer));
#endif
#if OUZEL_COMPILE_COREAUDIO
                case Driver::COREAUDIO:
                    engine->log(Log::Level::INFO) << "Using CoreAudio audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceCA(mixer));
#endif
#if OUZEL_COMPILE_ALSA
                case Driver::ALSA:
                    engine->log(Log::Level::INFO) << "Using ALSA audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceALSA(mixer));
#endif
#if OUZEL_COMPILE_WASAPI
                case Driver::WASAPI:
                    engine->log(Log::Level::INFO) << "Using WASAPI audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceWASAPI(mixer));
#endif
                default:
                    engine->log(Log::Level::INFO) << "Not using audio driver";
                    (void)debugAudio;
                    (void)window;
                    return std::unique_ptr<AudioDevice>(new AudioDeviceEmpty(mixer));
            }
        }

        Audio::Audio(Driver driver, bool debugAudio, Window* window):
            mixer(std::bind(&Audio::eventCallback, this, std::placeholders::_1)),
            masterMix(*this),
            device(createAudioDevice(driver, mixer, debugAudio, window))
        {
            mixer.addCommand(std::unique_ptr<Command>(new SetMasterBusCommand(masterMix.getBusId())));
        }

        Audio::~Audio()
        {
        }

        void Audio::update()
        {
            // TODO: handle events from the audio device
        }

        void Audio::deleteObject(uintptr_t objectId)
        {
            mixer.addCommand(std::unique_ptr<Command>(new DeleteObjectCommand(objectId)));
        }

        uintptr_t Audio::initBus()
        {
            uintptr_t busId = mixer.getObjectId();
            mixer.addCommand(std::unique_ptr<Command>(new InitBusCommand(busId)));
            return busId;
        }

        uintptr_t Audio::initSource(uintptr_t sourceDataId)
        {
            uintptr_t sourceId = mixer.getObjectId();
            mixer.addCommand(std::unique_ptr<Command>(new InitSourceCommand(sourceId, sourceDataId)));
            return sourceId;
        }

        uintptr_t Audio::initSourceData(std::unique_ptr<SourceData>&& sourceData)
        {
            uintptr_t sourceDataId = mixer.getObjectId();
            mixer.addCommand(std::unique_ptr<Command>(new InitSourceDataCommand(sourceDataId,
                                                                                std::forward<std::unique_ptr<SourceData>>(sourceData))));
            return sourceDataId;
        }

        uintptr_t Audio::initProcessor(std::unique_ptr<Processor>&& processor)
        {
            uintptr_t processorId = mixer.getObjectId();
            mixer.addCommand(std::unique_ptr<Command>(new InitProcessorCommand(processorId,
                                                                               std::forward<std::unique_ptr<Processor>>(processor))));
            return processorId;
        }

        void Audio::updateProcessor(uintptr_t processorId, const std::function<void(Processor*)>& updateFunction)
        {
            mixer.addCommand(std::unique_ptr<Command>(new UpdateProcessorCommand(processorId, updateFunction)));
        }

        void Audio::eventCallback(const Mixer::Event& event)
        {

        }
    } // namespace audio
} // namespace ouzel
