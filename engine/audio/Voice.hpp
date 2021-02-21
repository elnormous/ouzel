// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_VOICE_HPP
#define OUZEL_AUDIO_VOICE_HPP

#include <memory>
#include "Cue.hpp"
#include "Node.hpp"
#include "../math/Vector.hpp"

namespace ouzel::audio
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
        Voice(Audio& initAudio, const Sound* initSound);
        ~Voice() override;

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

        auto& getSound() const noexcept { return sound; }

        auto& getPosition() const noexcept { return position; }
        void setPosition(const Vector3F& newPosition) { position = newPosition; }

        auto& getVelocity() const noexcept { return velocity; }
        void setVelocity(const Vector3F& newVelocity) { velocity = newVelocity; }

        void play();
        void pause();
        void stop();

        auto isPlaying() const noexcept { return playing; }

        void setOutput(Mix* newOutput);

    private:
        Audio& audio;
        std::size_t streamId;

        const Sound* sound = nullptr;
        Vector3F position;
        Vector3F velocity;
        bool playing = false;

        Mix* output = nullptr;
    };
}

#endif // OUZEL_AUDIO_VOICE_HPP
