// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SOUND_HPP
#define OUZEL_AUDIO_SOUND_HPP

#include <memory>
#include "audio/Stream.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Mix;
        class SoundData;

        class Sound: public Stream::EventListener
        {
        public:
            Sound();
            explicit Sound(const std::shared_ptr<SoundData>& initSoundData);
            virtual ~Sound();

            inline const std::shared_ptr<SoundData>& getSoundData() const { return soundData; }

            void play(bool repeatSound = false);
            void pause();
            void stop();

            bool isPlaying() const { return playing; }
            bool isRepeating() const { return repeating; }

            void onReset() override;
            void onStop() override;

            void setOutput(Mix* newOutput);
            
        private:
            std::shared_ptr<SoundData> soundData;
            std::shared_ptr<Stream> stream;

            bool playing = false;
            bool repeating = false;

            Mix* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SOUND_HPP
