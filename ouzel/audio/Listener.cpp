// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Listener.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "core/Engine.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        Listener::Listener():
            scene::Component(scene::Component::LISTENER)
        {
        }

        Listener::~Listener()
        {
            if (audio) audio->removeListener(this);
        }

        AudioDevice::RenderCommand Listener::getRenderCommand()
        {
            AudioDevice::RenderCommand renderCommand;

            for (SoundInput* input : inputs)
            {
                renderCommand.renderCommands.push_back(input->getRenderCommand());
            }

            renderCommand.callback = std::bind(&Listener::render,
                                               std::placeholders::_1,
                                               std::placeholders::_2,
                                               std::placeholders::_3,
                                               std::placeholders::_4,
                                               std::placeholders::_5,
                                               position,
                                               rotation,
                                               rolloffFactor,
                                               minDistance,
                                               maxDistance);

            return renderCommand;
        }

        void Listener::draw(const Matrix4& transformMatrix,
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
            setRotation(transformMatrix.getRotation());
        }

        bool Listener::render(uint32_t,
                              uint16_t channels,
                              uint32_t,
                              Vector3& sourcePosition,
                              std::vector<float>& result,
                              const Vector3& listenerPosition,
                              const Quaternion& listenerRotation,
                              float rolloffFactor,
                              float minDistance,
                              float maxDistance)
        {
            Vector3 offset = sourcePosition - listenerPosition;
            float distance = clamp(offset.length(), minDistance, maxDistance);
            float attenuation = minDistance / (minDistance + rolloffFactor * (distance - minDistance)); // inverse distance

            std::vector<float> channelVolume(channels, attenuation);

            if (channelVolume.size() > 1)
            {
                Quaternion inverseRotation = -listenerRotation;
                Vector3 relative = inverseRotation * offset;
                relative.normalize();
                float angle = atan2f(relative.x, relative.z);

                // constant power panning
                channelVolume[0] = SQRT2 / 2.0f * (cosf(angle) - sinf(angle));
                channelVolume[1] = SQRT2 / 2.0f * (cosf(angle) + sinf(angle));
            }

            for (uint32_t frame = 0; frame < result.size() / channels; ++frame)
            {
                for (uint32_t channel = 0; channel < channels; ++channel)
                {
                    result[frame * channels + channel] *= channelVolume[channel];
                }
            }
            
            return true;
        }
    } // namespace audio
} // namespace ouzel
