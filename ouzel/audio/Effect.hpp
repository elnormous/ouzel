// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_EFFECT_HPP
#define OUZEL_AUDIO_EFFECT_HPP

#include <cstdint>

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Mix;

        class Effect
        {
            friend Mix;
        public:
            Effect(Audio& initAudio,
                   uintptr_t initProcessorId);
            virtual ~Effect();

            Effect(const Effect&) = delete;
            Effect& operator=(const Effect&) = delete;
            Effect(Effect&&) = delete;
            Effect& operator=(Effect&&) = delete;

            uintptr_t getProcessorId() const { return processorId; }

        protected:
            Audio& audio;
            uintptr_t processorId = 0;
            Mix* mix = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_EFFECT_HPP
