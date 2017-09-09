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

        AudioDevice::RenderCommand Listener::getRenderCommand()
        {
            AudioDevice::RenderCommand renderCommand;

            for (SoundInput* input : inputs)
            {
                renderCommand.renderCommands.push_back(input->getRenderCommand());
            }

            renderCommand.attributeCallback = std::bind(&Listener::setAttributes,
                                                        std::placeholders::_1,
                                                        std::placeholders::_2,
                                                        std::placeholders::_3,
                                                        std::placeholders::_4,
                                                        std::placeholders::_5,
                                                        position,
                                                        rotation);

            return renderCommand;
        }

        void Listener::setAttributes(Vector3& listenerPosition,
                                     Quaternion& listenerRotation,
                                     float&,
                                     float&,
                                     float&,
                                     const Vector3& position,
                                     const Quaternion& rotation)
        {
            listenerPosition = position;
            listenerRotation = rotation;
        }
    } // namespace audio
} // namespace ouzel
