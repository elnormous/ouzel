// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_ALSA

#include "AudioDeviceALSA.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDeviceALSA::AudioDeviceALSA():
            AudioDevice(Audio::Driver::ALSA), running(false)
        {
        }

        AudioDeviceALSA::~AudioDeviceALSA()
        {
            running = false;
            if (audioThread.isJoinable()) audioThread.join();

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

            Log(Log::Level::INFO) << "Using " << snd_pcm_name(playbackHandle) << " for audio";

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

            if (snd_pcm_hw_params_test_format(playbackHandle, hwParams, SND_PCM_FORMAT_FLOAT_LE) == 0)
            {
                if ((err = snd_pcm_hw_params_set_format(playbackHandle, hwParams, SND_PCM_FORMAT_FLOAT_LE)) < 0)
                {
                    Log(Log::Level::ERR) << "Failed to set sample format, error: " << err;
                    return false;
                }

                sampleFormat = Audio::SampleFormat::FLOAT32;
            }
            else if (snd_pcm_hw_params_test_format(playbackHandle, hwParams, SND_PCM_FORMAT_S16_LE) == 0)
            {
                if ((err = snd_pcm_hw_params_set_format(playbackHandle, hwParams, SND_PCM_FORMAT_S16_LE)) < 0)
                {
                    Log(Log::Level::ERR) << "Failed to set sample format, error: " << err;
                    return false;
                }

                sampleFormat = Audio::SampleFormat::SINT16;
            }
            else
            {
                Log(Log::Level::ERR) << "No supported format";
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

            unsigned int periodLength = periodSize * 1000000 / sampleRate; // period length in microseconds
            unsigned int bufferLength = periodLength * periods; // buffer length in microseconds
            int dir;

            if ((err = snd_pcm_hw_params_set_buffer_time_near(playbackHandle, hwParams, &bufferLength, &dir)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set buffer time, error: " << err;
                return false;
            }

            if ((err = snd_pcm_hw_params_set_period_time_near(playbackHandle, hwParams, &periodLength, &dir)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to set period time, error: " << err;
                return false;
            }

            if ((err = snd_pcm_hw_params_get_period_size(hwParams, &periodSize, &dir)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to get period size, error: " << err;
                return false;
            }

            if ((err = snd_pcm_hw_params_get_periods(hwParams, &periods, &dir)) < 0)
            {
                Log(Log::Level::ERR) << "Failed to get period count, error: " << err;
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

            snd_pcm_sw_params_free(swParams);
            swParams = nullptr;

            audioThread = Thread(std::bind(&AudioDeviceALSA::run, this), "Audio");

            return true;
        }

        void AudioDeviceALSA::run()
        {
            while (running)
            {
                int err;

                if (!process())
                {
                    break;
                }

                snd_pcm_sframes_t frames;

                if ((frames = snd_pcm_avail_update(playbackHandle)) < 0)
                {
                    if (frames == -EPIPE)
                    {
                        Log(Log::Level::WARN) << "Buffer underrun occurred";

                        if ((err = snd_pcm_prepare(playbackHandle)) < 0)
                        {
                            Log(Log::Level::ERR) << "Failed to prepare audio interface, error: " << err;
                            break;
                        }

                        continue;
                    }
                    else
                    {
                        Log(Log::Level::ERR) << "Failed to get available frames, error: " << frames;
                        break;
                    }
                }

                if (static_cast<snd_pcm_uframes_t>(frames) > periods * periodSize)
                {
                    Log(Log::Level::WARN) << "Buffer size exceeded, error: " << frames;
                    snd_pcm_reset(playbackHandle);
                    continue;
                }

                if (static_cast<snd_pcm_uframes_t>(frames) < periodSize)
                {
                    continue;
                }

                if (!getData(frames, data))
                {
                    break;
                }

                if ((err = snd_pcm_writei(playbackHandle, data.data(), frames)) < 0)
                {
                    if (err == -EPIPE)
                    {
                        Log(Log::Level::WARN) << "Buffer underrun occurred";

                        if ((err = snd_pcm_prepare(playbackHandle)) < 0)
                        {
                            Log(Log::Level::ERR) << "Failed to prepare audio interface, error: " << err;
                            break;
                        }
                    }
                    else
                    {
                        Log(Log::Level::ERR) << "Failed to write data, error: " << err;
                        break;
                    }
                }
            }
        }
    } // namespace audio
} // namespace ouzel

#endif
