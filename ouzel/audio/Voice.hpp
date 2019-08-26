// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_VOICE_HPP
#define OUZEL_AUDIO_VOICE_HPP

#include <memory>
#include "audio/Cue.hpp"
#include "audio/Node.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Mix;
        class Sound;

        class Voice final: public Node
        {
            friend Mix;
        public:
            explicit Voice(Audio& initAudio);
            Voice(Audio& initAudio, const Cue& cue);
            Voice(Audio& initAudio, const Sound& initSound);
            ~Voice();

            Voice(const Voice&) = delete;
            Voice& operator=(const Voice&) = delete;
            Voice(Voice&& other) noexcept:
                Node(other.audio),
                audio(other.audio)
            {
            }

            Voice& operator=(Voice&& other) noexcept
            {
                if (this == &other) return *this;

                return *this;
            }

            inline auto& getSound() const noexcept { return sound; }

            inline auto& getPosition() const noexcept { return position; }
            inline void setPosition(const Vector3F& newPosition) { position = newPosition; }

            inline auto& getVelocity() const noexcept { return velocity; }
            inline void setVelocity(const Vector3F& newVelocity) { velocity = newVelocity; }

            void play();
            void pause();
            void stop();

            inline auto isPlaying() const noexcept { return playing; }

            void setOutput(Mix* newOutput);

        private:
            Audio& audio;
            uintptr_t streamId;

            const Sound* sound = nullptr;
            Vector3F position;
            Vector3F velocity;
            bool playing = false;

            Mix* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_VOICE_HPP
