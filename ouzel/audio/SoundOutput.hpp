// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef SOUNDOUTPUT_HPP
#define SOUNDOUTPUT_HPP

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

#endif // SOUNDOUTPUT_HPP
