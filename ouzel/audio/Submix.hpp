// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SUBMIX_HPP
#define OUZEL_AUDIO_SUBMIX_HPP

#include "audio/Mix.hpp"

namespace ouzel
{
    namespace audio
    {
        struct SubmixDefinition final
        {
        };

        class Submix final: public Mix
        {
            friend Mix;
        public:
            explicit Submix(Audio& initAudio);
            Submix(Audio& initAudio, SubmixDefinition initSubmixDefinition);
            ~Submix();

            Submix(const Submix&) = delete;
            Submix& operator=(const Submix&) = delete;
            Submix(Submix&&) = delete;
            Submix& operator=(Submix&&) = delete;

            void setOutput(Mix* newOutput);

        private:
            SubmixDefinition submixDefinition;
            Mix* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SUBMIX_HPP
