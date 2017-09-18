// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sound.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "SoundData.hpp"
#include "Stream.hpp"
#include "core/Engine.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound():
            scene::Component(scene::Component::SOUND)
        {
            updateCallback.callback = std::bind(&Sound::update, this, std::placeholders::_1);
        }

        Sound::~Sound()
        {
        }

        bool Sound::init(const std::shared_ptr<SoundData>& newSoundData)
        {
            soundData = newSoundData;
            stream = soundData->createStream();
            resetCount = 0;

            return true;
        }

        void Sound::update(float)
        {
            if (stream)
            {
                uint32_t newResetCount = stream->getResetCount();

                for (uint32_t i = resetCount; i < newResetCount; ++i)
                {
                    Event resetEvent;
                    resetEvent.type = Event::Type::SOUND_RESET;
                    resetEvent.soundEvent.sound = this;
                    ++resetCount;
                }

                if (!stream->isPlaying())
                {
                    playing = false;

                    Event resetEvent;
                    resetEvent.type = Event::Type::SOUND_FINISH;
                    resetEvent.soundEvent.sound = this;

                    sharedEngine->unscheduleUpdate(&updateCallback);
                }

                if (actor) position = actor->getWorldPosition();
            }
            else
            {
                sharedEngine->unscheduleUpdate(&updateCallback);
            }
        }

        void Sound::setPosition(const Vector3& newPosition)
        {
            position = newPosition;
        }

        void Sound::setPitch(float newPitch)
        {
            pitch = newPitch;
        }

        void Sound::setGain(float newGain)
        {
            gain = newGain;
        }

        void Sound::setRolloffFactor(float newRolloffFactor)
        {
            rolloffFactor = newRolloffFactor;
        }

        void Sound::setMinDistance(float newMinDistance)
        {
            minDistance = newMinDistance;
        }

        void Sound::setMaxDistance(float newMaxDistance)
        {
            maxDistance = newMaxDistance;
        }

        void Sound::setSpatialized(bool newSpatialized)
        {
            spatialized = newSpatialized;
        }

        bool Sound::play(bool repeatSound)
        {
            if (actor) position = actor->getWorldPosition();

            playing = true;
            repeating = repeatSound;

            Event resetEvent;
            resetEvent.type = Event::Type::SOUND_START;
            resetEvent.soundEvent.sound = this;

            if (stream)
            {
                stream->setRepeating(repeatSound);
                stream->setPlaying(true);
            }

            sharedEngine->scheduleUpdate(&updateCallback);

            return true;
        }

        bool Sound::pause()
        {
            playing = false;
            if (stream) stream->setPlaying(false);

            sharedEngine->unscheduleUpdate(&updateCallback);

            return true;
        }

        bool Sound::stop()
        {
            playing = false;
            if (stream)
            {
                stream->setPlaying(false);
                stream->setShouldReset(true);
            }

            sharedEngine->unscheduleUpdate(&updateCallback);

            return true;
        }

        void Sound::addRenderCommands(std::vector<AudioDevice::RenderCommand>& renderCommands)
        {
            if (soundData && stream && stream->isPlaying())
            {
                AudioDevice::RenderCommand renderCommand;

                renderCommand.attributeCallback = std::bind(&Sound::setAttributes,
                                                            std::placeholders::_1,
                                                            std::placeholders::_2,
                                                            std::placeholders::_3,
                                                            std::placeholders::_4,
                                                            std::placeholders::_5,
                                                            pitch,
                                                            gain,
                                                            rolloffFactor);

                renderCommand.renderCallback = std::bind(&Sound::render,
                                                         std::placeholders::_1,
                                                         std::placeholders::_2,
                                                         std::placeholders::_3,
                                                         std::placeholders::_4,
                                                         std::placeholders::_5,
                                                         std::placeholders::_6,
                                                         std::placeholders::_7,
                                                         std::placeholders::_8,
                                                         std::placeholders::_9,
                                                         soundData,
                                                         stream,
                                                         position,
                                                         minDistance,
                                                         maxDistance,
                                                         spatialized);

                renderCommands.push_back(renderCommand);
            }
        }

        void Sound::setAttributes(Vector3&,
                                  Quaternion&,
                                  float& pitch,
                                  float& gain,
                                  float& rolloffFactor,
                                  float pitchScale,
                                  float gainScale,
                                  float rolloffScale)
        {
            pitch *= pitchScale;
            gain *= gainScale;
            rolloffFactor *= rolloffScale;
        }

        bool Sound::render(uint32_t frames,
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
                           bool spatialized)
        {
            if (soundData && soundData->getChannels() > 0 && stream)
            {
                if (!stream->isPlaying())
                {
                    result.clear();
                }
                else
                {
                    if (stream->getShouldReset())
                    {
                        stream->reset();
                        stream->setShouldReset(false);
                    }

                    soundData->getData(stream.get(), frames, channels, sampleRate, pitch, result);

                    std::vector<float> channelVolume(channels, gain);

                    if (spatialized)
                    {
                        Vector3 offset = position - listenerPosition;
                        float distance = clamp(offset.length(), minDistance, maxDistance);
                        float attenuation = minDistance / (minDistance + rolloffFactor * (distance - minDistance)); // inverse distance

                        for (float& volume : channelVolume)
                        {
                            volume *= attenuation;
                        }

                        if (channelVolume.size() > 1)
                        {
                            Quaternion inverseRotation = -listenerRotation;
                            Vector3 relative = inverseRotation * offset;
                            relative.normalize();
                            float angle = atan2f(relative.x, relative.z);
                            float c = cosf(angle);
                            float s = sinf(angle);

                            // constant power panning
                            channelVolume[0] *= clamp(SQRT2 / 2.0f * (c - s), 0.0f, 1.0f);
                            channelVolume[1] *= clamp(SQRT2 / 2.0f * (c + s), 0.0f, 1.0f);
                        }
                    }

                    for (uint32_t frame = 0; frame < result.size() / channels; ++frame)
                    {
                        for (uint32_t channel = 0; channel < channels; ++channel)
                        {
                            result[frame * channels + channel] *= channelVolume[channel];
                        }
                    }
                }
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
