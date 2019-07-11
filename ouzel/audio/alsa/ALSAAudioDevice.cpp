// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_ALSA

#include <system_error>
#include "ALSAAudioDevice.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace alsa
        {
            AudioDevice::AudioDevice(uint32_t initBufferSize,
                                     uint32_t initSampleRate,
                                     uint16_t initChannels,
                                     const std::function<void(uint32_t frames,
                                                              uint16_t channels,
                                                              uint32_t sampleRate,
                                                              std::vector<float>& samples)>& initDataGetter):
                audio::AudioDevice(Driver::ALSA, initBufferSize, initSampleRate, initChannels, initDataGetter)
            {
                int result;
                if ((result = snd_pcm_open(&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to connect to audio interface");

                engine->log(Log::Level::Info) << "Using " << snd_pcm_name(playbackHandle) << " for audio";

                if ((result = snd_pcm_hw_params_malloc(&hwParams)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to allocate memory for hardware parameters");

                if ((result = snd_pcm_hw_params_any(playbackHandle, hwParams)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to initialize hardware parameters");

                if ((result = snd_pcm_hw_params_set_access(playbackHandle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set access type");

                if (snd_pcm_hw_params_test_format(playbackHandle, hwParams, SND_PCM_FORMAT_FLOAT_LE) == 0)
                {
                    if ((result = snd_pcm_hw_params_set_format(playbackHandle, hwParams, SND_PCM_FORMAT_FLOAT_LE)) < 0)
                        throw std::system_error(result, std::system_category(), "Failed to set sample format");

                    sampleFormat = SampleFormat::Float32;
                }
                else if (snd_pcm_hw_params_test_format(playbackHandle, hwParams, SND_PCM_FORMAT_S16_LE) == 0)
                {
                    if ((result = snd_pcm_hw_params_set_format(playbackHandle, hwParams, SND_PCM_FORMAT_S16_LE)) < 0)
                        throw std::system_error(result, std::system_category(), "Failed to set sample format");

                    sampleFormat = SampleFormat::SInt16;
                }
                else
                    throw std::runtime_error("No supported format");

                if ((result = snd_pcm_hw_params_set_rate(playbackHandle, hwParams, sampleRate, 0)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set sample rate");

                if ((result = snd_pcm_hw_params_set_channels(playbackHandle, hwParams, channels)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set channel count");

                unsigned int periodLength = periodSize * 1000000 / sampleRate; // period length in microseconds
                unsigned int bufferLength = periodLength * periods; // buffer length in microseconds
                int dir;

                if ((result = snd_pcm_hw_params_set_buffer_time_near(playbackHandle, hwParams, &bufferLength, &dir)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set buffer time");

                if ((result = snd_pcm_hw_params_set_period_time_near(playbackHandle, hwParams, &periodLength, &dir)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set period time");

                if ((result = snd_pcm_hw_params_get_period_size(hwParams, &periodSize, &dir)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to get period size");

                if ((result = snd_pcm_hw_params_get_periods(hwParams, &periods, &dir)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to get period count");

                if ((result = snd_pcm_hw_params(playbackHandle, hwParams)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set hardware parameters");

                snd_pcm_hw_params_free(hwParams);
                hwParams = nullptr;

                if ((result = snd_pcm_sw_params_malloc(&swParams)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to allocate memory for software parameters");

                if ((result = snd_pcm_sw_params_current(playbackHandle, swParams)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to initialize software parameters");

                if ((result = snd_pcm_sw_params_set_avail_min(playbackHandle, swParams, 4096)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set minimum available count");

                if ((result = snd_pcm_sw_params_set_start_threshold(playbackHandle, swParams, 0)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set start threshold");

                if ((result = snd_pcm_sw_params(playbackHandle, swParams)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to set software parameters");

                if ((result = snd_pcm_prepare(playbackHandle)) < 0)
                    throw std::system_error(result, std::system_category(), "Failed to prepare audio interface");

                snd_pcm_sw_params_free(swParams);
                swParams = nullptr;
            }

            AudioDevice::~AudioDevice()
            {
                running = false;
                if (audioThread.joinable()) audioThread.join();

                if (swParams) snd_pcm_sw_params_free(swParams);
                if (hwParams) snd_pcm_hw_params_free(hwParams);
                if (playbackHandle) snd_pcm_close(playbackHandle);
            }

            void AudioDevice::start()
            {
                running = true;
                audioThread = std::thread(&AudioDevice::run, this);
            }

            void AudioDevice::stop()
            {
                running = false;
                if (audioThread.joinable()) audioThread.join();
            }

            void AudioDevice::run()
            {
                setCurrentThreadName("Audio");

                while (running)
                {
                    try
                    {
                        int result;

                        snd_pcm_sframes_t frames;

                        if ((frames = snd_pcm_avail_update(playbackHandle)) < 0)
                        {
                            if (frames == -EPIPE)
                            {
                                engine->log(Log::Level::Warning) << "Buffer underrun occurred";

                                if ((result = snd_pcm_prepare(playbackHandle)) < 0)
                                    throw std::system_error(result, std::system_category(), "Failed to prepare audio interface");

                                continue;
                            }
                            else
                                throw std::system_error(frames, std::system_category(), "Failed to get available frames");
                        }

                        if (static_cast<snd_pcm_uframes_t>(frames) > periods * periodSize)
                        {
                            engine->log(Log::Level::Warning) << "Buffer size exceeded, error: " << frames;
                            snd_pcm_reset(playbackHandle);
                            continue;
                        }

                        if (static_cast<snd_pcm_uframes_t>(frames) < periodSize)
                            continue;

                        getData(frames, data);

                        if ((result = snd_pcm_writei(playbackHandle, data.data(), frames)) < 0)
                        {
                            if (result == -EPIPE)
                            {
                                engine->log(Log::Level::Warning) << "Buffer underrun occurred";

                                if ((result = snd_pcm_prepare(playbackHandle)) < 0)
                                    throw std::system_error(result, std::system_category(), "Failed to prepare audio interface");
                            }
                            else
                                throw std::system_error(result, std::system_category(), "Failed to write data");
                        }
                    }
                    catch (const std::exception& e)
                    {
                        engine->log(Log::Level::Error) << e.what();
                    }
                }
            }
        } // namespace alsa
    } // namespace audio
} // namespace ouzel

#endif
