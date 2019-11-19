// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SOURCE_HPP
#define OUZEL_AUDIO_SOURCE_HPP

#include <memory>
#include <vector>
#include "audio/Effect.hpp"

namespace ouzel
{
    namespace audio
    {
        class Source
        {
        public:
            virtual ~Source() = default;

            void play()
            {
                playing = true;
            }

            auto& getEffects() const noexcept { return effects; }
            void stop(bool shouldReset)
            {
                playing = false;

                if (shouldReset)
                    reset();
            }

            virtual void reset() = 0;
            virtual void getSamples(uint32_t frames, uint32_t channels, uint32_t sampleRate, std::vector<float>& samples) = 0;

            bool isPlaying() const noexcept { return playing; }

        private:
            std::vector<std::unique_ptr<Effect>> effects;
            bool playing = false;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SOURCE_HPP
