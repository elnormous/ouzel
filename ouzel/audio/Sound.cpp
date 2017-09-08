// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sound.hpp"
#include "SoundResource.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound():
            scene::Component(scene::Component::SOUND)
        {
            resource = sharedEngine->getAudio()->getDevice()->createSound();
        }

        Sound::~Sound()
        {
            if (sharedEngine) sharedEngine->getAudio()->getDevice()->deleteResource(resource);
        }

        bool Sound::init(const std::shared_ptr<SoundData>& newSoundData)
        {
            soundData = newSoundData;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::init,
                                                                     resource,
                                                                     newSoundData));

            return true;
        }

        void Sound::draw(const Matrix4& transformMatrix,
                         float opacity,
                         const Matrix4& renderViewProjection,
                         const std::shared_ptr<graphics::Texture>& renderTarget,
                         const Rectangle& renderViewport,
                         bool depthWrite,
                         bool depthTest,
                         bool wireframe,
                         bool scissorTest,
                         const Rectangle& scissorRectangle)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            renderTarget,
                            renderViewport,
                            depthWrite,
                            depthTest,
                            wireframe,
                            scissorTest,
                            scissorRectangle);

            setPosition(transformMatrix.getTranslation());
        }

        void Sound::setPosition(const Vector3& newPosition)
        {
            position = newPosition;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setPosition,
                                                                     resource,
                                                                     newPosition));
        }

        void Sound::setPitch(float newPitch)
        {
            pitch = newPitch;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setPitch,
                                                                     resource,
                                                                     newPitch));
        }

        void Sound::setGain(float newGain)
        {
            gain = newGain;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setGain,
                                                                     resource,
                                                                     newGain));
        }

        void Sound::setRolloffFactor(float newRolloffFactor)
        {
            rolloffFactor = newRolloffFactor;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setGain,
                                                                     resource,
                                                                     newRolloffFactor));
        }

        void Sound::setMinDistance(float newMinDistance)
        {
            minDistance = newMinDistance;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setMinDistance,
                                                                     resource,
                                                                     newMinDistance));
        }

        void Sound::setMaxDistance(float newMaxDistance)
        {
            maxDistance = newMaxDistance;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::setMaxDistance,
                                                                     resource,
                                                                     newMaxDistance));
        }

        bool Sound::play(bool repeatSound)
        {
            repeating = repeatSound;

            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::play,
                                                                     resource,
                                                                     repeatSound));
            return true;
        }

        bool Sound::pause()
        {
            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::pause,
                                                                     resource));

            return true;
        }

        bool Sound::stop()
        {
            sharedEngine->getAudio()->executeOnAudioThread(std::bind(&SoundResource::stop,
                                                                     resource));

            return true;
        }

        AudioDevice::RenderCommand Sound::getRenderCommand()
        {
            AudioDevice::RenderCommand renderCommand;

            renderCommand.callback = std::bind(&SoundResource::getData,
                                               resource,
                                               std::placeholders::_1,
                                               std::placeholders::_2,
                                               std::placeholders::_3,
                                               std::placeholders::_4,
                                               std::placeholders::_5);

            return renderCommand;
        }
    } // namespace audio
} // namespace ouzel
