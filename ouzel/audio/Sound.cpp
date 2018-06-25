// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
        }

        Sound::~Sound()
        {
            if (stream) stream->setEventListener(nullptr);
        }

        void Sound::init(const std::shared_ptr<SoundData>& newSoundData)
        {
            soundData = newSoundData;
            if (soundData)
            {
                stream = soundData->createStream();
                stream->setEventListener(this);
            }
        }

        void Sound::updateTransform()
        {
            transformDirty = true;
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

            return true;
        }

        bool Sound::pause()
        {
            playing = false;
            if (stream) stream->setPlaying(false);

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

            return true;
        }

        void Sound::addRenderCommands(std::vector<AudioDevice::RenderCommand>& renderCommands)
        {
            if (soundData && stream && stream->isPlaying())
            {
                if (transformDirty && actor)
                {
                    position = actor->getWorldPosition();
                    transformDirty = false;
                }

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

        // executed on audio thread
        void Sound::onReset()
        {
            Event event;
            event.type = Event::Type::SOUND_RESET;
            event.soundEvent.sound = this;
            engine->getEventDispatcher()->postEvent(event);
        }

        // executed on audio thread
        void Sound::onStop()
        {
            engine->getSceneManager()->executeOnUpdateThread([this]() {
                playing = false;

                Event event;
                event.type = Event::Type::SOUND_FINISH;
                event.soundEvent.sound = this;
                engine->getEventDispatcher()->postEvent(event);
            });
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
            if (soundData && stream)
            {
                if (!stream->isPlaying())
                    result.clear();
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
                            volume *= attenuation;

                        if (channelVolume.size() > 1)
                        {
                            Quaternion inverseRotation = listenerRotation;
                            inverseRotation.invert();
                            Vector3 relative = inverseRotation * offset;
                            relative.normalize();
                            float angle = atan2f(relative.x, relative.z);
                            float c = cosf(angle);
                            float s = sinf(angle);

                            // constant power panning
                            channelVolume[0] *= clamp(SQRT2 / 2.0F * (c - s), 0.0F, 1.0F);
                            channelVolume[1] *= clamp(SQRT2 / 2.0F * (c + s), 0.0F, 1.0F);
                        }
                    }

                    for (uint32_t frame = 0; frame < result.size() / channels; ++frame)
                        for (uint32_t channel = 0; channel < channels; ++channel)
                            result[frame * channels + channel] *= channelVolume[channel];
                }
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
