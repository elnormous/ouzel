// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_DESTINATION_HPP
#define OUZEL_AUDIO_DESTINATION_HPP

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

        private:
            void addInput(SoundInput* input);
            void removeInput(SoundInput* input);

            std::vector<SoundInput*> inputs;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_DESTINATION_HPP
