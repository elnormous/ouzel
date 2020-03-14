// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_EFFECT_HPP
#define OUZEL_AUDIO_EFFECT_HPP

#include <cstdint>
#include "audio/Node.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Mix;

        class Effect: public Node
        {
            friend Mix;
        public:
            Effect(Audio& initAudio,
                   std::uintptr_t initProcessorId);
            ~Effect() override;

            Effect(const Effect&) = delete;
            Effect& operator=(const Effect&) = delete;
            Effect(Effect&&) = delete;
            Effect& operator=(Effect&&) = delete;

            inline auto getProcessorId() const noexcept { return processorId; }

            inline auto isEnabled() const noexcept { return enabled; }
            void setEnabled(bool newEnabled);

        protected:
            Audio& audio;
            std::uintptr_t processorId = 0;
            Mix* mix = nullptr;
            bool enabled = true;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_EFFECT_HPP
