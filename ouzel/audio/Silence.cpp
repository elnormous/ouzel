// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Silence.hpp"
#include "Audio.hpp"
#include "mixer/Data.hpp"
#include "mixer/Stream.hpp"

namespace ouzel
{
    namespace audio
    {
        Silence::Silence(float initLength):
            length(initLength)
        {
        }

        void Silence::getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)
        {
            std::fill(samples.begin(), samples.end(), 0.0F); // TODO: fill only the needed samples

            if (length > 0.0F)
            {
                const auto frameCount = static_cast<uint32_t>(length * sampleRate);
                auto neededSize = frames;

                if (neededSize > 0)
                {
                    if (frameCount - position < neededSize)
                    {
                        neededSize -= frameCount - position;
                        position = frameCount;
                    }
                    else
                    {
                        position += neededSize;
                        neededSize = 0;
                    }
                }

                if ((frameCount - position) == 0)
                {
                    //playing = false; // TODO: fire event
                    //reset();
                }
            }
            else
            {
                position += frames;
            }
        }
    } // namespace audio
} // namespace ouzel
