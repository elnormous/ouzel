// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_STREAM_HPP
#define OUZEL_AUDIO_MIXER_STREAM_HPP

#include "audio/mixer/Object.hpp"

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
                Stream(Data& initData);
                ~Stream();

                const Data& getData() const { return data; }

                virtual void getData(uint32_t frames, std::vector<float>& samples) = 0;

                void setOutput(Bus* newOutput);

                inline bool isPlaying() const { return playing; }
                void play();

                void stop(bool shouldReset);
                virtual void reset() = 0;

            protected:
                Data& data;
                Bus* output = nullptr;
                bool playing = false;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_STREAM_HPP
