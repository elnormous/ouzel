// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#if OUZEL_PLATFORM_IOS
#include "audio/openal/ios/AudioDeviceALIOS.hpp"
#elif OUZEL_PLATFORM_TVOS
#include "audio/openal/tvos/AudioDeviceALTVOS.hpp"
#endif
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "alsa/AudioDeviceALSA.hpp"
#include "coreaudio/AudioDeviceCA.hpp"
#include "dsound/AudioDeviceDS.hpp"
#include "empty/AudioDeviceEmpty.hpp"
#include "openal/AudioDeviceAL.hpp"
#include "opensl/AudioDeviceSL.hpp"
#include "xaudio2/AudioDeviceXA2.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        std::set<Audio::Driver> Audio::getAvailableAudioDrivers()
        {
            static std::set<Driver> availableDrivers;

            if (availableDrivers.empty())
            {
                availableDrivers.insert(Audio::Driver::EMPTY);

#if OUZEL_SUPPORTS_OPENAL
                availableDrivers.insert(Audio::Driver::OPENAL);
#endif

#if OUZEL_SUPPORTS_DIRECTSOUND
                availableDrivers.insert(Audio::Driver::DIRECTSOUND);
#endif

#if OUZEL_SUPPORTS_XAUDIO2
                availableDrivers.insert(Audio::Driver::XAUDIO2);
#endif

#if OUZEL_SUPPORTS_OPENSL
                availableDrivers.insert(Audio::Driver::OPENSL);
#endif
#if OUZEL_SUPPORTS_COREAUDIO
                availableDrivers.insert(Audio::Driver::COREAUDIO);
#endif
#if OUZEL_SUPPORTS_ALSA
                availableDrivers.insert(Audio::Driver::ALSA);
#endif
            }
            
            return availableDrivers;
        }

        Audio::Audio(Driver driver)
        {
            switch (driver)
            {
#if OUZEL_SUPPORTS_OPENAL
                case Driver::OPENAL:
                    Log(Log::Level::INFO) << "Using OpenAL audio driver";
    #if OUZEL_PLATFORM_IOS
                    device.reset(new AudioDeviceALIOS());
    #elif OUZEL_PLATFORM_TVOS
                    device.reset(new AudioDeviceALTVOS());
    #else
                    device.reset(new AudioDeviceAL());
    #endif
                    break;
#endif
#if OUZEL_SUPPORTS_DIRECTSOUND
                case Driver::DIRECTSOUND:
                    Log(Log::Level::INFO) << "Using DirectSound audio driver";
                    device.reset(new AudioDeviceDS());
                    break;
#endif
#if OUZEL_SUPPORTS_XAUDIO2
                case Driver::XAUDIO2:
                    Log(Log::Level::INFO) << "Using XAudio 2 audio driver";
                    device.reset(new AudioDeviceXA2());
                    break;
#endif
#if OUZEL_SUPPORTS_OPENSL
                case Driver::OPENSL:
                    Log(Log::Level::INFO) << "Using OpenSL ES audio driver";
                    device.reset(new AudioDeviceSL());
                    break;
#endif
#if OUZEL_SUPPORTS_COREAUDIO
                case Driver::COREAUDIO:
                    Log(Log::Level::INFO) << "Using CoreAudio audio driver";
                    device.reset(new AudioDeviceCA());
                    break;
#endif
#if OUZEL_SUPPORTS_ALSA
                case Driver::ALSA:
                    Log(Log::Level::INFO) << "Using ALSA audio driver";
                    device.reset(new AudioDeviceALSA());
                    break;
#endif
                case Driver::EMPTY:
                default:
                    Log(Log::Level::INFO) << "Not using audio driver";
                    device.reset(new AudioDeviceEmpty());
                    break;
            }
        }

        Audio::~Audio()
        {
        }

        bool Audio::init(bool debugAudio)
        {
            device->init(debugAudio);
            return true;
        }

        bool Audio::update()
        {
            return device->update();
        }

        void Audio::setListenerPosition(const Vector3& newPosition)
        {
            listenerPosition = newPosition;

            executeOnAudioThread(std::bind(&AudioDevice::setListenerPosition, device.get(), newPosition));
        }

        void Audio::setListenerRotation(const Quaternion& newRotation)
        {
            listenerRotation = newRotation;

            executeOnAudioThread(std::bind(&AudioDevice::setListenerRotation, device.get(), newRotation));
        }

        void Audio::executeOnAudioThread(const std::function<void(void)>& func)
        {
            device->executeOnAudioThread(func);
        }
    } // namespace audio
} // namespace ouzel
