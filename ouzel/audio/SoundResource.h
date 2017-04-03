// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include <memory>
#include "utils/Noncopyable.h"
#include "audio/Resource.h"
#include "math/Vector3.h"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class SoundData;
        
        class SoundResource: public Resource, public Noncopyable
        {
            friend Audio;
        public:
            virtual ~SoundResource();

            virtual bool init(const std::shared_ptr<SoundData>& newSoundData);

            const std::shared_ptr<SoundData>& getSoundData() const { return soundData; }

            virtual void setPosition(const Vector3& newPosition);
            virtual void setPitch(float newPitch);
            virtual void setGain(float newGain);

            virtual bool play(bool repeatSound = false);
            virtual bool stop(bool resetSound = false);
            virtual bool reset();

            bool isRepeating() const { return repeat; }

        protected:
            SoundResource();

            enum Dirty
            {
                DIRTY_SOUND_DATA = 0x01,
                DIRTY_POSITION = 0x02,
                DIRTY_PITCH = 0x04,
                DIRTY_GAIN = 0x08,
                DIRTY_PLAY_STATE = 0x10
            };

            uint32_t dirty = 0;

            std::shared_ptr<SoundData> soundData;
            bool repeat = false;

            Vector3 position;
            float pitch = 1.0f;
            float gain = 1.0f;

            std::mutex uploadMutex;
        };
    } // namespace audio
} // namespace ouzel
