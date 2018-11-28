// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef AUDIO_SUBMIX_HPP
#define AUDIO_SUBMIX_HPP

#include <cstdint>

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Submix final
        {
        public:
            Submix(Audio& initAudio);
            void setOutput(Submix* newOutput);

        private:
            Audio& audio;
            uintptr_t nodeId;
            Submix* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // AUDIO_SUBMIX_HPP
