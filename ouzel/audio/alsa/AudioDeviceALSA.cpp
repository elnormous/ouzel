// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_ALSA

#include "AudioDeviceALSA.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDeviceALSA::AudioDeviceALSA():
            AudioDevice(Audio::Driver::ALSA)
        {
        }

        AudioDeviceALSA::~AudioDeviceALSA()
        {
            running = false;
            if (audioThread.joinable()) audioThread.join();

            if (swParams) snd_pcm_sw_params_free(swParams);
            if (hwParams) snd_pcm_hw_params_free(hwParams);
            if (playbackHandle) snd_pcm_close(playbackHandle);
        }

        bool AudioDeviceALSA::init(bool debugAudio)
        {
            if (!AudioDevice::init(debugAudio))
            {
                return false;
            }

            int err;
            if ((err = snd_pcm_open(&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to connect to audio interface, error: " << err;
                return false;
            }          

            if ((err = snd_pcm_hw_params_malloc(&hwParams)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to allocate memory for hardware parameters, error: " << err;
                return false;
            }
                     
            if ((err = snd_pcm_hw_params_any(playbackHandle, hwParams)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to initialize hardware parameters, error: " << err;
                return false;
            }
        
            if ((err = snd_pcm_hw_params_set_access(playbackHandle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set access type, error: " << err;
                return false;
            }
        
            if ((err = snd_pcm_hw_params_set_format(playbackHandle, hwParams, SND_PCM_FORMAT_S16_LE)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set sample format, error: " << err;
                return false;
            }
        
            if ((err = snd_pcm_hw_params_set_rate(playbackHandle, hwParams, sampleRate, 0)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set sample rate, error: " << err;
                return false;
            }
        
            if ((err = snd_pcm_hw_params_set_channels(playbackHandle, hwParams, channels)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set channel count, error: " << err;
                return false;
            }
        
            if ((err = snd_pcm_hw_params(playbackHandle, hwParams)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set hardware parameters, error: " << err;
                return false;
            }

            snd_pcm_hw_params_free(hwParams);
            hwParams = nullptr;

            if ((err = snd_pcm_sw_params_malloc(&swParams)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to allocate memory for software parameters, error: " << err;
                return false;
            }

            if ((err = snd_pcm_sw_params_current(playbackHandle, swParams)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to initialize software parameters, error: " << err;
                return false;
            }

            if ((err = snd_pcm_sw_params_set_avail_min(playbackHandle, swParams, 4096)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set minimum available count, error: " << err;
                return false;
            }

            if ((err = snd_pcm_sw_params_set_start_threshold(playbackHandle, swParams, 0)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set start threshold, error: " << err;
                return false;
            }

            if ((err = snd_pcm_sw_params(playbackHandle, swParams)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set software parameters, error: " << err;
                return false;
            }
        
            if ((err = snd_pcm_prepare(playbackHandle)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to prepare audio interface, error: " << err;
                return false;
            }

            audioThread = std::thread(&AudioALSA::run, this);

            return true;
        }

        void AudioDeviceALSA::run()
        {
            while (running)
            {
                int err;

                if ((err = snd_pcm_wait(playbackHandle, 1000)) < 0)
                {
                    Log(Log::Level::ERR) << "Failed to poll, error: " << err;
			        break;
                }

                snd_pcm_sframes_t frames;

                if ((frames = snd_pcm_avail_update(playbackHandle)) < 0)
                {
                    if (frames == -EPIPE)
                    {
                        Log(Log::Level::WARN) << "Buffer underrun occurred";
                    }
                    else
                    {
                        Log(Log::Level::ERR) << "Failed to get available frames, error: " << frames;
                        break;
                    }
                }
        
                frames = frames > 4096 ? 4096 : frames;
        
                if (!getData(frames / 2, Format::SINT16, data))
                {
                    break;
                }

                if ((err = snd_pcm_writei(playbackHandle, data.data(), frames)) < 0)
                {
                    Log(Log::Level::ERR) << "Failed to write data, error: " << err;
                }
            }
        }
    } // namespace audio
} // namespace ouzel

#endif
