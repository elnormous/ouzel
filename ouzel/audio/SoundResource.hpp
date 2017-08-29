// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include "utils/Noncopyable.hpp"
#include "math/Vector3.hpp"

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

            virtual bool init(const std::shared_ptr<SoundData>& newSoundData, bool newRelativePosition = false);

            virtual void setPosition(const Vector3& newPosition);
            virtual void setPitch(float newPitch);
            virtual void setGain(float newGain);

            virtual bool play(bool repeatSound = false);
            virtual bool pause();
            virtual bool stop();

            bool isRepeating() const { return repeat; }

            bool getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& result);

        protected:
            std::shared_ptr<SoundData> soundData;
            std::unique_ptr<Stream> stream;
            bool relativePosition = false;
            bool shouldPlay = false;
            bool repeat = false;
            bool reset = false;

            Vector3 position;
            float pitch = 1.0f;
            float gain = 1.0f;

            std::vector<float> data;
        };
    } // namespace audio
} // namespace ouzel
