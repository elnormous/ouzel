// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cfloat>
#include <atomic>
#include <memory>
#include "audio/SoundInput.hpp"
#include "audio/Stream.hpp"
#include "scene/Component.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class SoundData;

        class Sound: public SoundInput, public Stream::EventListener, public scene::Component
        {
            friend Audio;
        public:
            Sound();
            Sound(const std::shared_ptr<SoundData>& initSoundData);
            virtual ~Sound();

            Sound(Sound&& sound);
            Sound& operator=(Sound&& sound);

            inline const std::shared_ptr<SoundData>& getSoundData() const { return soundData; }

            inline float getPitch() const { return pitch; }
            void setPitch(float newPitch);

            inline float getGain() const { return gain; }
            void setGain(float newGain);

            inline float getRolloffFactor() const { return rolloffFactor; }
            void setRolloffFactor(float newRolloffFactor);

            inline float getMinDistance() const { return minDistance; }
            void setMinDistance(float newMinDistance);

            inline float getMaxDistance() const { return maxDistance; }
            void setMaxDistance(float newMaxDistance);

            inline bool isSpatialized() const { return spatialized; }
            void setSpatialized(bool newSpatialized);

            void play(bool repeatSound = false);
            void pause();
            void stop();

            bool isPlaying() const { return playing; }
            bool isRepeating() const { return repeating; }

            virtual void addRenderCommands(std::vector<AudioDevice::RenderCommand>& renderCommands) override;

            virtual void onReset() override;
            virtual void onStop() override;

        private:
            virtual void updateTransform() override;

            static void setAttributes(Vector3& listenerPosition,
                                      Quaternion& listenerRotation,
                                      float& pitch,
                                      float& gain,
                                      float& rolloffFactor,
                                      float pitchScale,
                                      float gainScale,
                                      float rolloffScale);

            static void render(uint32_t frames,
                               uint16_t channels,
                               uint32_t sampleRate,
                               const Vector3& listenerPosition,
                               const Quaternion& listenerRotation,
                               float pitch,
                               float gain,
                               float rolloffFactor,
                               std::vector<float>& result,
                               const std::shared_ptr<SoundData>& soundData,
                               const std::shared_ptr<Stream>& stream,
                               const Vector3& position,
                               float minDistance,
                               float maxDistance,
                               bool spatialized);

            std::shared_ptr<SoundData> soundData;
            std::shared_ptr<Stream> stream;

            Vector3 position;
            float pitch = 1.0F;
            float gain = 1.0F;
            float rolloffFactor = 1.0F;
            float minDistance = 1.0F;
            float maxDistance = FLT_MAX;

            bool transformDirty = true;
            bool spatialized = true;
            std::atomic<bool> playing; // TODO: remove atomic
            bool repeating = false;
        };
    } // namespace audio
} // namespace ouzel
