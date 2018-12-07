// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Source.hpp"
#include "Bus.hpp"
#include "SourceData.hpp"

namespace ouzel
{
    namespace audio
    {
        Source::Source(SourceData& initSourceData):
            sourceData(initSourceData)
        {
        }

        Source::~Source()
        {
            if (output) output->removeInput(this);
        }

        void Source::sampleData(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                                Vector3 listenerPosition, std::vector<float>& samples)
        {
            getData(frames, buffer);

            if (sampleRate != sourceData.getSampleRate())
            {
                // TODO: resample
            }

            uint32_t sourceChannels = sourceData.getChannels();

            samples.resize(frames * channels);
            
            if (sourceChannels != channels)
            {
                switch (sourceChannels)
                {
                    case 1:
                    {
                        switch (channels)
                        {
                            case 2: // upmix 1 to 2
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = buffer[frame]; // L = M
                                    samples[frame * channels + 1] = buffer[frame]; // R = M
                                }
                                break;
                            case 4: // upmix 1 to 4
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = buffer[frame]; // L = M
                                    samples[frame * channels + 1] = buffer[frame]; // R = M
                                    samples[frame * channels + 2] = 0.0F; // SL = 0
                                    samples[frame * channels + 3] = 0.0F; // SR = 0
                                }
                                break;
                            case 6: // upmix 1 to 6
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = 0.0F; // L = 0
                                    samples[frame * channels + 1] = 0.0F; // R = 0
                                    samples[frame * channels + 2] = buffer[frame]; // C = M
                                    samples[frame * channels + 3] = 0.0F; // LFE = 0
                                    samples[frame * channels + 4] = 0.0F; // SL = 0
                                    samples[frame * channels + 5] = 0.0F; // SR = 0
                                }
                                break;
                        }
                        break;
                    }
                    case 2:
                    {
                        switch (channels)
                        {
                            case 1: // downmix 2 to 1
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                    samples[frame] = (buffer[frame * sourceChannels + 0] +
                                                      buffer[frame * sourceChannels + 1]) * 0.5F; // M = (L + R) * 0.5
                                break;
                            case 4: // upmix 2 to 4
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = buffer[frame * sourceChannels + 0]; // L = L
                                    samples[frame * channels + 1] = buffer[frame * sourceChannels + 1]; // R = R
                                    samples[frame * channels + 2] = 0.0F; // SL = 0
                                    samples[frame * channels + 3] = 0.0F; // SR = 0
                                }
                                break;
                            case 6: // upmix 2 to 6
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = buffer[frame * sourceChannels + 0]; // L = L
                                    samples[frame * channels + 1] = buffer[frame * sourceChannels + 1]; // R = R
                                    samples[frame * channels + 2] = 0.0F; // C = 0
                                    samples[frame * channels + 3] = 0.0F; // LFE = 0
                                    samples[frame * channels + 4] = 0.0F; // SL = 0
                                    samples[frame * channels + 5] = 0.0F; // SR = 0
                                }
                                break;
                        }
                        break;
                    }
                    case 4:
                    {
                        switch (channels)
                        {
                            case 1: // downmix 4 to 1
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                    samples[frame] = (buffer[frame * sourceChannels + 0] +
                                                      buffer[frame * sourceChannels + 1] +
                                                      buffer[frame * sourceChannels + 2] +
                                                      buffer[frame * sourceChannels + 3]) * 0.25F; // M = (L + R + SL + SR) * 0.25
                                break;
                            case 2: // downmix 4 to 2
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = (buffer[frame * sourceChannels + 0] +
                                                                     buffer[frame * sourceChannels + 2]) * 0.5F; // L = (L + SL) * 0.5
                                    samples[frame * channels + 1] = (buffer[frame * sourceChannels + 1] +
                                                                     buffer[frame * sourceChannels + 3]) * 0.5F; // R = (R + RL) * 0.5
                                }
                                break;
                            case 6: // upmix 4 to 6
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = buffer[frame * sourceChannels + 0]; // L = L
                                    samples[frame * channels + 1] = buffer[frame * sourceChannels + 1]; // R = R
                                    samples[frame * channels + 2] = 0.0F; // C = 0
                                    samples[frame * channels + 3] = 0.0F; // LFE = 0
                                    samples[frame * channels + 4] = buffer[frame * sourceChannels + 2]; // SL = SL
                                    samples[frame * channels + 5] = buffer[frame * sourceChannels + 3]; // SR = SR
                                }
                                break;
                        }
                        break;
                    }
                    case 6:
                    {
                        switch (channels)
                        {
                            case 1: // downmix 6 to 1
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                    samples[frame] = ((buffer[frame * sourceChannels + 0] +
                                                       buffer[frame * sourceChannels + 1]) * 0.7071F +
                                                      buffer[frame * sourceChannels + 2] +
                                                      (buffer[frame * sourceChannels + 4] +
                                                       buffer[frame * sourceChannels + 5]) * 0.5F); // M = (L + R) * 0.7071 + C + (SL + SR) * 0.5
                                break;
                            case 2: // downmix 6 to 2
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = (buffer[frame * sourceChannels + 0] +
                                                                     (buffer[frame * sourceChannels + 2] +
                                                                      buffer[frame * sourceChannels + 4]) * 0.7071F); // L = L + (C + SL) * 0.7071
                                    samples[frame * channels + 1] = (buffer[frame * sourceChannels + 1] +
                                                                     (buffer[frame * sourceChannels + 2] +
                                                                      buffer[frame * sourceChannels + 5]) * 0.7071F); // R = R + (C + SR) * 0.7071
                                }
                                break;
                            case 4: // downmix 6 to 4
                                for (uint32_t frame = 0; frame < frames; ++frame)
                                {
                                    samples[frame * channels + 0] = (buffer[frame * sourceChannels + 0] +
                                                                     buffer[frame * sourceChannels + 2] * 0.7071F); // L = L + C * 0.7071
                                    samples[frame * channels + 1] = (buffer[frame * sourceChannels + 1] +
                                                                     buffer[frame * sourceChannels + 2] * 0.7071F); // R = R + C * 0.7071
                                    samples[frame * channels + 2] = buffer[frame * sourceChannels + 4]; // SL = SL
                                    samples[frame * channels + 3] = buffer[frame * sourceChannels + 5]; // SR = SR
                                }
                                break;
                        }
                        break;
                    }
                }
            }
            else
                samples = buffer;
        }

        void Source::setOutput(Bus* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);
        }

        void Source::play(bool repeat)
        {
            playing = true;
            repeating = repeat;
        }

        void Source::stop(bool shouldReset)
        {
            playing = false;
            if (shouldReset) reset();
        }
    } // namespace audio
} // namespace ouzel
