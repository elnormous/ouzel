// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_DESTINATION_HPP
#define OUZEL_AUDIO_DESTINATION_HPP

#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Destination
        {
            friend Audio;
        public:
            Destination(Audio& initAudio, uintptr_t initDestinationId);
            virtual ~Destination();

            Destination(const Destination&) = delete;
            Destination& operator=(const Destination&) = delete;

            Destination(Destination&&) = delete;
            Destination& operator=(Destination&&) = delete;

            inline uintptr_t getDestinationId() const { return destinationId; }

        protected:
            Audio& audio;
            uintptr_t destinationId = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_DESTINATION_HPP
