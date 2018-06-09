// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "core/Setup.h"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Listener.hpp"
#include "alsa/AudioDeviceALSA.hpp"
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
        std::set<Audio::Driver> Audio::getAvailableAudioDrivers()
        {
            static std::set<Driver> availableDrivers;

            if (availableDrivers.empty())
            {
                availableDrivers.insert(Audio::Driver::EMPTY);

#if OUZEL_COMPILE_OPENAL
                availableDrivers.insert(Audio::Driver::OPENAL);
#endif

#if OUZEL_COMPILE_DIRECTSOUND
                availableDrivers.insert(Audio::Driver::DIRECTSOUND);
#endif

#if OUZEL_COMPILE_XAUDIO2
                availableDrivers.insert(Audio::Driver::XAUDIO2);
#endif

#if OUZEL_COMPILE_OPENSL
                availableDrivers.insert(Audio::Driver::OPENSL);
#endif
#if OUZEL_COMPILE_COREAUDIO
                availableDrivers.insert(Audio::Driver::COREAUDIO);
#endif
#if OUZEL_COMPILE_ALSA
                availableDrivers.insert(Audio::Driver::ALSA);
#endif
            }

            return availableDrivers;
        }

        Audio::Audio(Driver driver)
        {
            switch (driver)
            {
#if OUZEL_COMPILE_OPENAL
                case Driver::OPENAL:
                    Log(Log::Level::INFO) << "Using OpenAL audio driver";
                    device.reset(new AudioDeviceAL());
                    break;
#endif
#if OUZEL_COMPILE_DIRECTSOUND
                case Driver::DIRECTSOUND:
                    Log(Log::Level::INFO) << "Using DirectSound audio driver";
                    device.reset(new AudioDeviceDS());
                    break;
#endif
#if OUZEL_COMPILE_XAUDIO2
                case Driver::XAUDIO2:
                    Log(Log::Level::INFO) << "Using XAudio 2 audio driver";
                    device.reset(new AudioDeviceXA2());
                    break;
#endif
#if OUZEL_COMPILE_OPENSL
                case Driver::OPENSL:
                    Log(Log::Level::INFO) << "Using OpenSL ES audio driver";
                    device.reset(new AudioDeviceSL());
                    break;
#endif
#if OUZEL_COMPILE_COREAUDIO
                case Driver::COREAUDIO:
                    Log(Log::Level::INFO) << "Using CoreAudio audio driver";
                    device.reset(new AudioDeviceCA());
                    break;
#endif
#if OUZEL_COMPILE_ALSA
                case Driver::ALSA:
                    Log(Log::Level::INFO) << "Using ALSA audio driver";
                    device.reset(new AudioDeviceALSA());
                    break;
#endif
                default:
                    Log(Log::Level::INFO) << "Not using audio driver";
                    device.reset(new AudioDeviceEmpty());
                    break;
            }
        }

        Audio::~Audio()
        {
            for (Listener* listener : listeners)
                listener->audio = nullptr;
        }

        bool Audio::init(bool debugAudio)
        {
            device->init(debugAudio);
            return true;
        }

        bool Audio::update()
        {
            std::vector<AudioDevice::RenderCommand> renderCommands;

            for (Listener* listener : listeners)
                renderCommands.push_back(listener->getRenderCommand());

            device->setRenderCommands(renderCommands);

            return true;
        }

        void Audio::executeOnAudioThread(const std::function<void(void)>& func)
        {
            device->executeOnAudioThread(func);
        }

        void Audio::addListener(Listener* listener)
        {
            if (listener->audio != this)
            {
                if (listener->audio) listener->audio->removeListener(listener);
                listeners.push_back(listener);
                listener->audio = this;
            }
        }

        void Audio::removeListener(Listener* listener)
        {
            if (listener->audio == this)
            {
                listener->audio = nullptr;

                auto i = std::find(listeners.begin(), listeners.end(), listener);

                if (i != listeners.end())
                    listeners.erase(i);
            }
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
