// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        bool SoundResource::init(const std::shared_ptr<SoundData>& newSoundData)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            soundData = newSoundData;
            ready = true;

            return true;
        }

        void SoundResource::setPosition(const Vector3& newPosition)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            position = newPosition;
        }

        void SoundResource::setPitch(float newPitch)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            pitch = newPitch;
        }

        void SoundResource::setGain(float newGain)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            gain = newGain;
        }

        bool SoundResource::play(bool repeatSound)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            repeat = repeatSound;

            stop(true);

            return true;
        }

        bool SoundResource::stop(bool resetSound)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (resetSound)
            {
                reset();
            }

            return true;
        }
        
        bool SoundResource::reset()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);
            
            return true;
        }
    } // namespace audio
} // namespace ouzel
