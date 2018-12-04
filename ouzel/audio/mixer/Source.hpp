// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_SOURCE_HPP
#define OUZEL_AUDIO_MIXER_SOURCE_HPP

#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        class Bus;

        class Source: public Object
        {
            friend Bus;
        public:
            ~Source();

            void setOutput(Bus* newOutput);

            virtual void getData(std::vector<float>& samples, uint16_t& channels,
                                 uint32_t& sampleRate, Vector3& position) = 0;

            bool isPlaying() const { return playing; }
            void play(bool repeat);

            bool isRepeating() const { return repeating; }
            void stop(bool shouldReset);
            virtual void reset() = 0;

        private:
            Bus* output = nullptr;
            bool playing = false;
            bool repeating = false;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_SOURCE_HPP
