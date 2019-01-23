// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_VOICE_HPP
#define OUZEL_AUDIO_VOICE_HPP

#include <memory>
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Mix;
        class Sound;

        class Voice final
        {
            friend Mix;
        public:
            Voice(Audio& initAudio, const std::shared_ptr<Sound>& initSound);
            ~Voice();

            Voice(const Voice&) = delete;
            Voice& operator=(const Voice&) = delete;
            Voice(Voice&&) = delete;
            Voice& operator=(Voice&&) = delete;

            inline const std::shared_ptr<Sound>& getSound() const { return sound; }

            void play(bool repeat = false);
            void pause();
            void stop();

            bool isPlaying() const { return playing; }
            bool isRepeating() const { return repeating; }

            void setOutput(Mix* newOutput);

        private:
            Audio& audio;
            uintptr_t streamId;

            std::shared_ptr<Sound> sound;

            bool playing = false;
            bool repeating = false;

            Mix* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_VOICE_HPP
