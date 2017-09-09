// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sound.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "SoundData.hpp"
#include "Stream.hpp"
#include "core/Engine.hpp"
#include "math/MathUtils.hpp"

static void resampleLerp(const std::vector<float>& src, uint32_t srcFrames,
                         std::vector<float>& dst, uint32_t dstFrames,
                         uint32_t channels)
{
    if (dstFrames > 0) // do resampling only if setination is not empty
    {
        if (srcFrames == 0) // source is empty
        {
            dst.resize(dstFrames * channels);

            std::fill(dst.begin(), dst.end(), 0.0f);
        }
        else
        {
            float srcIncrement = static_cast<float>(srcFrames - 1) / static_cast<float>(dstFrames - 1);
            float srcPosition = 0.0f;

            dst.resize(dstFrames * channels);

            for (uint32_t frame = 0; frame < dstFrames - 1; ++frame)
            {
                uint32_t srcCurrentFrame = static_cast<uint32_t>(srcPosition);
                float frac = srcPosition - srcCurrentFrame;

                srcPosition += srcIncrement;
                uint32_t srcNextFrame = srcCurrentFrame + 1;

                for (uint32_t channel = 0; channel < channels; ++channel)
                {
                    uint32_t srcCurrentPosition = srcCurrentFrame * channels + channel;
                    uint32_t srcNextPosition = srcNextFrame * channels + channel;

                    dst[frame * channels + channel] = ouzel::lerp(src[srcCurrentPosition], src[srcNextPosition], frac);
                }
            }

            // fill the last frame of the destination with the last frame of the source
            for (uint32_t channel = 0; channel < channels; ++channel)
            {
                dst[(dstFrames - 1) * channels + channel] = src[(srcFrames - 1) * channels + channel];
            }
        }
    }
}

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
                Event resetEvent;
                resetEvent.type = Event::Type::SOUND_FINISH;
                resetEvent.soundEvent.sound = this;

                sharedEngine->unscheduleUpdate(&updateCallback);
            }
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

        bool Sound::play(bool repeatSound)
        {
            Event resetEvent;
            resetEvent.type = Event::Type::SOUND_START;
            resetEvent.soundEvent.sound = this;
            ++resetCount;

            stream->setRepeating(repeatSound);
            stream->setPlaying(true);

            sharedEngine->scheduleUpdate(&updateCallback);

            return true;
        }

        bool Sound::pause()
        {
            stream->setPlaying(false);

            sharedEngine->unscheduleUpdate(&updateCallback);

            return true;
        }

        bool Sound::stop()
        {
            stream->setPlaying(false);

            sharedEngine->unscheduleUpdate(&updateCallback);

            //sharedEngine->getAudio()->executeOnAudioThread(std::bind(&Stream::reset,
            //                                                         stream.get()));

            return true;
        }

        AudioDevice::RenderCommand Sound::getRenderCommand()
        {
            AudioDevice::RenderCommand renderCommand;

            renderCommand.callback = std::bind(&Sound::render,
                                               std::placeholders::_1,
                                               std::placeholders::_2,
                                               std::placeholders::_3,
                                               std::placeholders::_4,
                                               std::placeholders::_5,
                                               std::placeholders::_6,
                                               std::placeholders::_7,
                                               std::placeholders::_8,
                                               soundData,
                                               stream,
                                               position,
                                               pitch,
                                               gain,
                                               rolloffFactor,
                                               minDistance,
                                               maxDistance);

            return renderCommand;
        }

        bool Sound::render(uint32_t frames,
                           uint16_t channels,
                           uint32_t sampleRate,
                           Vector3& sourcePosition,
                           float& sourceRolloffFactor,
                           float& sourceMinDistance,
                           float& sourceMaxDistance,
                           std::vector<float>& result,
                           const std::shared_ptr<SoundData>& soundData,
                           const std::shared_ptr<Stream>& stream,
                           const Vector3& position,
                           float pitch,
                           float gain,
                           float rolloffFactor,
                           float minDistance,
                           float maxDistance)
        {
            sourcePosition = position;
            sourceRolloffFactor = rolloffFactor;
            sourceMinDistance = minDistance;
            sourceMaxDistance = maxDistance;

            if (!stream->isPlaying())
            {
                result.clear();
            }
            else if (soundData && soundData->getChannels() > 0 && stream)
            {
                std::vector<float> data;
                std::vector<float> resampledData;

                float finalPitch = pitch * soundData->getSampleRate() / sampleRate;
                uint32_t neededFrames = static_cast<uint32_t>(frames * finalPitch);

                if (!soundData->getData(stream.get(), neededFrames, data))
                {
                    return false;
                }

                if (finalPitch != 1.0f)
                {
                    uint32_t srcFrames = static_cast<uint32_t>(data.size()) / soundData->getChannels();
                    uint32_t dstFrames = static_cast<uint32_t>(frames * (static_cast<float>(srcFrames) / neededFrames));

                    resampleLerp(data, srcFrames, resampledData, dstFrames, soundData->getChannels());
                }
                else
                {
                    resampledData = data;
                }

                if (channels != soundData->getChannels())
                {
                    uint32_t srcFrames = static_cast<uint32_t>(resampledData.size()) / soundData->getChannels();

                    result.resize(srcFrames * channels);

                    // front left channel
                    if (channels >= 1)
                    {
                        uint32_t destination = 0;

                        if (soundData->getChannels() >= 1)
                        {
                            uint32_t source = 0;

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination] = resampledData[source];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // fill the front left channel with zeros
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination] = 0.0f;
                                destination += channels;
                            }
                        }
                    }

                    // front right channel
                    if (channels >= 2)
                    {
                        uint32_t destination = 0;

                        // sound data has front right channel
                        if (soundData->getChannels() >= 2)
                        {
                            uint32_t source = 0;

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 1] = resampledData[source + 1];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the front right one
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 1] = result[destination];
                                destination += channels;
                            }
                        }
                    }

                    // center channel
                    if (channels >= 3)
                    {
                        uint32_t destination = 0;

                        if (soundData->getChannels() >= 3)
                        {
                            uint32_t source = 0;

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 2] = resampledData[source + 2];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else if (channels >= 2)
                        {
                            // calculate the average of the front left and the front right channel
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 2] = (result[destination] + result[destination + 1]) / 2.0f;
                                destination += channels;
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the center one
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 2] = result[destination];
                                destination += channels;
                            }
                        }
                    }

                    // LFE channel
                    if (channels >= 4)
                    {
                        uint32_t destination = 0;

                        if (soundData->getChannels() >= 4)
                        {
                            uint32_t source = 0;

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 3] = resampledData[source + 3];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // fill the LFE channel with zeros
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 3] = 0;
                                destination += channels;
                            }
                        }
                    }

                    // back left channel
                    if (channels >= 5)
                    {
                        uint32_t destination = 0;

                        // sound data has back left channel
                        if (soundData->getChannels() >= 5)
                        {
                            uint32_t source = 0;

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 4] = resampledData[source + 4];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front left channel in to the back left one
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 4] = result[destination];
                                destination += channels;
                            }
                        }
                    }

                    // back right channel
                    if (channels >= 6)
                    {
                        uint32_t destination = 0;

                        // sound data has back right channel
                        if (soundData->getChannels() >= 6)
                        {
                            uint32_t source = 0;

                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 5] = resampledData[source + 5];
                                destination += channels;
                                source += soundData->getChannels();
                            }
                        }
                        else
                        {
                            // copy the front right channel in to the back right one
                            for (uint32_t frame = 0; frame < srcFrames; ++frame)
                            {
                                result[destination + 5] = result[destination + 1];
                                destination += channels;
                            }
                        }
                    }
                }
                else
                {
                    result = resampledData;
                }

                if (gain != 1.0f)
                {
                    for (float& sample : result)
                    {
                        sample *= gain;
                    }
                }
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
