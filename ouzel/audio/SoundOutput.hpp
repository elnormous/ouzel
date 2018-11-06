// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SOUNDOUTPUT_HPP
#define OUZEL_SOUNDOUTPUT_HPP

#include <vector>

namespace ouzel
{
    namespace audio
    {
        class SoundInput;

        class SoundOutput
        {
            friend SoundInput;
        public:
            virtual ~SoundOutput();

        protected:
            void addInput(SoundInput* input);
            void removeInput(SoundInput* input);

            std::vector<SoundInput*> inputs;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_SOUNDOUTPUT_HPP
