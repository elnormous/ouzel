// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "math/Vector3.h"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class SoundData;

        class Sound
        {
            friend Audio;
        public:
            virtual ~Sound();

            virtual bool init(const std::shared_ptr<SoundData>& newSoundData);

            const std::shared_ptr<SoundData>& getSoundData() const { return soundData; }

            virtual void setPosition(const Vector3& newPosition);
            virtual void setPitch(float newPitch);
            virtual void setGain(float newGain);

            virtual bool play(bool repeatSound = false);
            virtual bool stop(bool resetSound = false);
            virtual bool reset();

            bool isRepeating() const { return repeat; }

            bool isReady() const { return ready; }

        protected:
            Sound();

            std::shared_ptr<SoundData> soundData;
            bool repeat = false;

            bool ready = false;

            Vector3 position;
            float pitch = 1.0f;
            float gain = 1.0f;
        };
    } // namespace audio
} // namespace ouzel
