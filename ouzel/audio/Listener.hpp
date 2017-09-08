// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cfloat>
#include "utils/Noncopyable.hpp"
#include "audio/AudioDevice.hpp"
#include "audio/SoundOutput.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Listener: public Noncopyable, public SoundOutput
        {
            friend Audio;
        public:
            Listener();
            virtual ~Listener();

            const Vector3& getPosition() const { return position; }
            void setPosition(const Vector3& newPosition) { position = newPosition; }

            const Quaternion& getRotation() const { return rotation; }
            void setRotation(const Quaternion& newRotation) { rotation = newRotation; }

            AudioDevice::RenderCommand getRenderCommand();

        protected:
            static bool render(uint32_t frames,
                               uint16_t channels,
                               uint32_t sampleRate,
                               Vector3& sourcePosition,
                               std::vector<float>& result,
                               const Vector3& listenerPosition,
                               const Quaternion& listenerRotation,
                               float rolloffFactor,
                               float minDistance,
                               float maxDistance);

            Audio* audio = nullptr;

            Vector3 position;
            Quaternion rotation;

            float rolloffFactor = 1.0f;
            float minDistance = 1.0f;
            float maxDistance = FLT_MAX;
        };
    } // namespace audio
} // namespace ouzel
