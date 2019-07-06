// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_STREAM_HPP
#define OUZEL_AUDIO_MIXER_STREAM_HPP

#include "audio/mixer/Object.hpp"
#include "audio/mixer/Bus.hpp"
#include "audio/mixer/Data.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Bus;
            class Data;

            class Stream: public Object
            {
                friend Bus;
            public:
                Stream(Data& initData):
                    data(initData)
                {
                }

                ~Stream()
                {
                    if (output) output->removeInput(this);
                }

                const Data& getData() const { return data; }

                void setOutput(Bus* newOutput)
                {
                    if (output) output->removeInput(this);
                    output = newOutput;
                    if (output) output->addInput(this);
                }

                inline auto isPlaying() const { return playing; }
                void play() { playing = true; }

                void stop(bool shouldReset)
                {
                    playing = false;
                    if (shouldReset) reset();
                }

                virtual void reset() = 0;

                virtual void getSamples(uint32_t frames, std::vector<float>& samples) = 0;

            protected:
                Data& data;
                Bus* output = nullptr;
                bool playing = false;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_STREAM_HPP
