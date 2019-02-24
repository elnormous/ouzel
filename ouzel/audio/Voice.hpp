// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_VOICE_HPP
#define OUZEL_AUDIO_VOICE_HPP

#include <memory>
#include "math/Vector.hpp"

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

            const Vector3F& getPosition() const { return position; }
            void setPosition(const Vector3F& newPosition) { position = newPosition; }

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
            Vector3F position;

            bool playing = false;
            bool repeating = false;

            Mix* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_VOICE_HPP
