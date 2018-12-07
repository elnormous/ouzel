// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_SOURCE_HPP
#define OUZEL_AUDIO_MIXER_SOURCE_HPP

#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        class Bus;
        class SourceData;

        class Source: public Object
        {
            friend Bus;
        public:
            Source(SourceData& initSourceData);
            ~Source();

            const SourceData& getSourceData() const { return sourceData; }

            virtual void getData(uint32_t frames, std::vector<float>& samples) = 0;

            void setOutput(Bus* newOutput);

            bool isPlaying() const { return playing; }
            void play(bool repeat);

            bool isRepeating() const { return repeating; }
            void stop(bool shouldReset);
            virtual void reset() = 0;

        protected:
            SourceData& sourceData;
            Bus* output = nullptr;
            bool playing = false;
            bool repeating = false;
            std::vector<float> resampleBuffer;
            std::vector<float> buffer;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_SOURCE_HPP
