// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include <memory>
#include <vector>
#include "utils/Noncopyable.h"
#include "math/Vector3.h"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class SoundData;
        class Stream;
        
        class SoundResource: public Noncopyable
        {
            friend Audio;
        public:
            SoundResource();
            virtual ~SoundResource();

            virtual bool init(const std::shared_ptr<SoundData>& newSoundData, bool streaming);

            virtual void setPosition(const Vector3& newPosition);
            virtual void setPitch(float newPitch);
            virtual void setGain(float newGain);

            virtual bool play(bool repeatSound = false);
            virtual bool pause();
            virtual bool stop();

            bool isRepeating() const { return repeat; }

            std::vector<uint8_t> getData(uint32_t size, uint16_t channels, uint32_t samplesPerSecond);

        protected:
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
            std::unique_ptr<Stream> stream;
            bool shouldPlay = false;
            bool repeat = false;
            bool reset = false;

            Vector3 position;
            float pitch = 1.0f;
            float gain = 1.0f;

            std::mutex uploadMutex;
        };
    } // namespace audio
} // namespace ouzel
