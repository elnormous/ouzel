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
        class SoundResource;

        class Sound
        {
            friend Audio;
        public:
            Sound();
            virtual ~Sound();

            virtual bool init(const std::shared_ptr<SoundData>& newSoundData, bool relativePosition = false);

            const std::shared_ptr<SoundData>& getSoundData() const { return soundData; }

            virtual void setPosition(const Vector3& newPosition);
            virtual void setPitch(float newPitch);
            virtual void setGain(float newGain);

            virtual bool play(bool repeatSound = false);
            virtual bool pause();
            virtual bool stop();

            bool isRepeating() const;

        protected:
            SoundResource* resource = nullptr;

            std::shared_ptr<SoundData> soundData;
            bool repeating = false;
        };
    } // namespace audio
} // namespace ouzel
