// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_DESTINATION_HPP
#define OUZEL_AUDIO_DESTINATION_HPP

#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Source;

        class Destination
        {
            friend Source;
        public:
            virtual ~Destination();

        private:
            void addInput(Source* input);
            void removeInput(Source* input);

            std::vector<Source*> inputs;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_DESTINATION_HPP
