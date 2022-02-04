// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_ALSA

#include <system_error>
#include "ALSAAudioDevice.hpp"
#include "ALSAErrorCategory.hpp"
#include "../../core/Engine.hpp"
#include "../../utils/Log.hpp"

namespace ouzel::audio::alsa
{
    AudioDevice::AudioDevice(const Settings& settings,
                             const std::function<void(std::uint32_t frames,
                                                      std::uint32_t channels,
                                                      std::uint32_t sampleRate,
                                                      std::vector<float>& samples)>& initDataGetter):
        audio::AudioDevice{Driver::alsa, settings, initDataGetter}
    {
        const char* device = settings.audioDevice.empty() ? "default" : settings.audioDevice.c_str();

        if (const auto result = snd_pcm_open(&playbackHandle, device, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK); result != 0)
            throw std::system_error{result, errorCategory, "Failed to connect to audio interface"};

        log(Log::Level::info) << "Using " << snd_pcm_name(playbackHandle) << " for audio";

        snd_pcm_hw_params_t* hwParams = nullptr;
        snd_pcm_hw_params_alloca(&hwParams);

        if (const auto result = snd_pcm_hw_params_any(playbackHandle, hwParams); result < 0)
            throw std::system_error{result, errorCategory, "Failed to initialize hardware parameters"};

        if (const auto result = snd_pcm_hw_params_set_access(playbackHandle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set access type"};

        if (snd_pcm_hw_params_test_format(playbackHandle, hwParams, SND_PCM_FORMAT_FLOAT_LE) == 0)
        {
            if (const auto result = snd_pcm_hw_params_set_format(playbackHandle, hwParams, SND_PCM_FORMAT_FLOAT_LE); result != 0)
                throw std::system_error{result, errorCategory, "Failed to set sample format"};

            sampleFormat = SampleFormat::float32;
        }
        else if (snd_pcm_hw_params_test_format(playbackHandle, hwParams, SND_PCM_FORMAT_S16_LE) == 0)
        {
            if (const auto result = snd_pcm_hw_params_set_format(playbackHandle, hwParams, SND_PCM_FORMAT_S16_LE); result != 0)
                throw std::system_error{result, errorCategory, "Failed to set sample format"};

            sampleFormat = SampleFormat::signedInt16;
        }
        else
            throw std::runtime_error{"No supported format"};

        if (const auto result = snd_pcm_hw_params_set_rate(playbackHandle, hwParams, sampleRate, 0); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set sample rate"};

        if (const auto result = snd_pcm_hw_params_set_channels(playbackHandle, hwParams, channels); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set channel count"};

        unsigned int periodLength = periodSize * 1000000U / sampleRate; // period length in microseconds
        unsigned int bufferLength = periodLength * periods; // buffer length in microseconds
        int dir;

        if (const auto result = snd_pcm_hw_params_set_buffer_time_near(playbackHandle, hwParams, &bufferLength, &dir); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set buffer time"};

        if (const auto result = snd_pcm_hw_params_set_period_time_near(playbackHandle, hwParams, &periodLength, &dir); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set period time"};

        if (const auto result = snd_pcm_hw_params_get_period_size(hwParams, &periodSize, &dir); result != 0)
            throw std::system_error{result, errorCategory, "Failed to get period size"};

        if (const auto result = snd_pcm_hw_params_get_periods(hwParams, &periods, &dir); result != 0)
            throw std::system_error{result, errorCategory, "Failed to get period count"};

        if (const auto result = snd_pcm_hw_params(playbackHandle, hwParams); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set hardware parameters"};

        snd_pcm_sw_params_t* swParams = nullptr;
        snd_pcm_sw_params_alloca(&swParams);

        if (const auto result = snd_pcm_sw_params_current(playbackHandle, swParams); result != 0)
            throw std::system_error{result, errorCategory, "Failed to initialize software parameters"};

        if (const auto result = snd_pcm_sw_params_set_avail_min(playbackHandle, swParams, 4096); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set minimum available count"};

        if (const auto result = snd_pcm_sw_params_set_start_threshold(playbackHandle, swParams, 0); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set start threshold"};

        if (const auto result = snd_pcm_sw_params(playbackHandle, swParams); result != 0)
            throw std::system_error{result, errorCategory, "Failed to set software parameters"};

        if (const auto result = snd_pcm_prepare(playbackHandle); result != 0)
            throw std::system_error{result, errorCategory, "Failed to prepare audio interface"};
    }

    AudioDevice::~AudioDevice()
    {
        running = false;
        if (audioThread.isJoinable()) audioThread.join();

        if (playbackHandle) snd_pcm_close(playbackHandle);
    }

    void AudioDevice::start()
    {
        running = true;
        audioThread = thread::Thread{&AudioDevice::run, this};
    }

    void AudioDevice::stop()
    {
        running = false;
        if (audioThread.isJoinable()) audioThread.join();
    }

    void AudioDevice::run()
    {
        while (running)
        {
            try
            {
                snd_pcm_sframes_t frames;

                if ((frames = snd_pcm_avail_update(playbackHandle)) < 0)
                {
                    if (frames == -EPIPE)
                    {
                        log(Log::Level::warning) << "Buffer underrun occurred";

                        if (const auto result = snd_pcm_prepare(playbackHandle); result != 0)
                            throw std::system_error{result, errorCategory, "Failed to prepare audio interface"};

                        continue;
                    }
                    else
                        throw std::system_error{frames, errorCategory, "Failed to get available frames"};
                }

                if (static_cast<snd_pcm_uframes_t>(frames) > periods * periodSize)
                {
                    log(Log::Level::warning) << "Buffer size exceeded, error: " << frames;
                    snd_pcm_reset(playbackHandle);
                    continue;
                }

                if (static_cast<snd_pcm_uframes_t>(frames) < periodSize)
                    continue;

                getData(frames, data);

                if (const auto result = snd_pcm_writei(playbackHandle, data.data(), frames); result != 0)
                {
                    if (result == -EPIPE)
                    {
                        log(Log::Level::warning) << "Buffer underrun occurred";

                        if (const auto prepareResult = snd_pcm_prepare(playbackHandle); prepareResult != 0)
                            throw std::system_error{prepareResult, errorCategory, "Failed to prepare audio interface"};
                    }
                    else
                        throw std::system_error{result, errorCategory, "Failed to write data"};
                }
            }
            catch (const std::exception& e)
            {
                log(Log::Level::error) << e.what();
            }
        }
    }
}
#endif
